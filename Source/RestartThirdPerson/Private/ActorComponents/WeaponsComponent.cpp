// Brandon Hillig 2026

#include "ActorComponents/WeaponsComponent.h"

#include "MetasoundSource.h"
#include "NiagaraFunctionLibrary.h"
#include "ActorComponents/AttributesComponent.h"
#include "Actors/WeaponPickup.h"
#include "Kismet/GameplayStatics.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

UWeaponsComponent::UWeaponsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponsComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponInventory.Add(EWeapon::Unarmed, UnarmedWeapon);

	if (AActor* Owner = GetOwner())
	{
		IWeaponAimSource* AimSource = Cast<IWeaponAimSource>(Owner);
		WeaponAimSource.SetObject(Owner);
		WeaponAimSource.SetInterface(AimSource);
		ensureMsgf(WeaponAimSource, TEXT("%s requires its owner to implement IWeaponAimSource"), *GetName());
	}
}


bool UWeaponsComponent::CanEquipWeaponType(EWeapon WeaponType) const
{
	// If we are reloading we can not switch yet
	if (WeaponInventory.Contains(EquippedWeaponType) && WeaponInventory[EquippedWeaponType].bIsReloading)
	{
		return false;
	}

	if (WeaponSwapPhase != EWeaponSwapPhase::None)
	{
		return false;
	}

	if (WeaponType == EWeapon::Unarmed)
	{
		return true;
	}

	return WeaponInventory.Contains(WeaponType);
}

void UWeaponsComponent::EquipWeaponType(EWeapon WeaponType)
{
	// Check if we need to unequip first
	if (EquippedWeaponType != EWeapon::Unarmed)
	{
		UnequipCurrentWeapon();

		// Set pending weapon to equip
		PendingWeaponType = WeaponType;
		return;
	}

	EquipWeapon(WeaponType);
}

bool UWeaponsComponent::CanReloadEquippedWeapon() const
{
	if (WeaponSwapPhase != EWeaponSwapPhase::None)
	{
		return false;
	}

	const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponType];
	if (EquippedWeapon.bIsReloading)
	{
		return false;
	}

	// For each weapon we can reload if we have bullets outside our current clip and our current clip isn't full
	return EquippedWeapon.TotalBullets > EquippedWeapon.CurrentBulletsInClip && EquippedWeapon.CurrentBulletsInClip != EquippedWeapon.Config.BulletsPerClip;
}

void UWeaponsComponent::ReloadEquippedWeapon()
{
	FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponType];
	const FWeaponConfig& WeaponConfig = EquippedWeapon.Config;
	const EWeapon WeaponType = WeaponConfig.WeaponType;

	EquippedWeapon.bIsReloading = true;

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, WeaponType]()
		{
			if (FWeapon* Weapon = WeaponInventory.Find(WeaponType))
			{
				Weapon->bIsReloading = false;
				// Take as much ammo from remaining bullets as possible (up to clip amount)
				const int32 BulletsOutsideClip = Weapon->TotalBullets - Weapon->CurrentBulletsInClip;
				const int32 BulletsInNewClip = FMath::Min(Weapon->Config.BulletsPerClip - Weapon->CurrentBulletsInClip, BulletsOutsideClip);
				Weapon->CurrentBulletsInClip += BulletsInNewClip;
				OnWeaponAmmoChanged.Broadcast(Weapon->CurrentBulletsInClip, Weapon->TotalBullets);
			}
		});


	GetOwner()->GetWorldTimerManager().SetTimer(EquippedWeapon.TimerHandle_Reload, Delegate, WeaponConfig.ReloadDuration, false);

	OnWeaponAnimationRequested.Broadcast(EquippedWeaponType, WeaponConfig.ReloadAnim, WeaponConfig.CharacterReloadAnimMontage);
}

void UWeaponsComponent::FireWeapon()
{
	const EWeaponFireState WeaponFireState = GetWeaponFireState();
	if (WeaponFireState == EWeaponFireState::NoAmmoInClip && CanReloadEquippedWeapon())
	{
		ReloadEquippedWeapon();
		return;
	}
	if (WeaponFireState != EWeaponFireState::CanFire || WeaponSwapPhase != EWeaponSwapPhase::None)
	{
		return;
	}

	FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponType];
	const FWeaponConfig& WeaponConfig = EquippedWeapon.Config;
	const EWeapon WeaponType = WeaponConfig.WeaponType;

	EquippedWeapon.bFireIntervalElapsed = false;

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, WeaponType]()
		{
			if (FWeapon* Weapon = WeaponInventory.Find(WeaponType))
			{
				Weapon->bFireIntervalElapsed = true;
			}
		});

	const USkeletalMeshComponent* WeaponMesh = FindWeaponMesh(WeaponConfig);
	check(WeaponMesh);

	const FVector WeaponBarrelLocation = WeaponMesh->GetBoneLocation(WeaponConfig.MuzzleSocketName);

	GetOwner()->GetWorldTimerManager().SetTimer(EquippedWeapon.TimerHandle_FireInterval, Delegate, WeaponConfig.FireInterval, false);

	OnWeaponAnimationRequested.Broadcast(EquippedWeaponType, WeaponConfig.FireAnim, WeaponConfig.CharacterFireAnimMontage);

	// Play Fire Sound
	UGameplayStatics::PlaySoundAtLocation(this, WeaponConfig.FireSound, WeaponBarrelLocation);

	EquippedWeapon.CurrentBulletsInClip--;
	EquippedWeapon.TotalBullets--;
	OnWeaponAmmoChanged.Broadcast(EquippedWeapon.CurrentBulletsInClip, EquippedWeapon.TotalBullets);

	FVector WeaponOrigin;
	FVector WeaponDirection;
	if (WeaponAimSource)
	{
		WeaponAimSource->GetWeaponAimRay(WeaponOrigin, WeaponDirection);
	}

	// Perform a raycast to see if we hit something
	const FVector Start = WeaponOrigin;
	const FVector End = Start + WeaponDirection * 10'000;
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult CameraHitResult;
	GetWorld()->LineTraceSingleByChannel(CameraHitResult, Start, End, ECC_Visibility, QueryParams);

	const FVector PotentialImpactPoint = CameraHitResult.bBlockingHit ? CameraHitResult.ImpactPoint : End;

	FHitResult GunBarrelToImpactPointHitResult;
	GetWorld()->LineTraceSingleByChannel(GunBarrelToImpactPointHitResult, WeaponBarrelLocation, PotentialImpactPoint, ECC_Visibility, QueryParams);

	const FVector ImpactPoint = GunBarrelToImpactPointHitResult.bBlockingHit ? GunBarrelToImpactPointHitResult.ImpactPoint : PotentialImpactPoint;

	// If we hit any object
	if (CameraHitResult.bBlockingHit || GunBarrelToImpactPointHitResult.bBlockingHit)
	{
		const FHitResult& HitResultToUse = GunBarrelToImpactPointHitResult.bBlockingHit ? GunBarrelToImpactPointHitResult : CameraHitResult;
		AActor* HitActor = HitResultToUse.GetActor();

		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		AController* EventInstigator = InstigatorPawn ? InstigatorPawn->GetController() : nullptr;

		TArray<UAttributesComponent*> AttributeComponents;
		HitActor->GetComponents(UAttributesComponent::StaticClass(), AttributeComponents);
		UAttributesComponent* AttributeComp = !AttributeComponents.IsEmpty() ? AttributeComponents[0] : nullptr;

		const float OldHealth = AttributeComp ? AttributeComp->GetHealth() : 0.f;

		// Apply damage
		// TODO: Allow weapons to specify their damage type class
		UGameplayStatics::ApplyPointDamage(HitActor, WeaponConfig.DamagePerBullet, WeaponDirection, HitResultToUse, EventInstigator, GetOwner(), UDamageType::StaticClass());

		const float NewHealth = AttributeComp ? AttributeComp->GetHealth() : 0.f;

		const float DamageDealt = OldHealth - NewHealth;

		if (AttributeComp && DamageDealt > 0.f)
		{
			// We hit an object with an attributes component so broadcast a hitmarker
			const EHitMarkerType HitMarkerType = FMath::IsNearlyZero(NewHealth) ? EHitMarkerType::Kill : EHitMarkerType::Base;
			OnHitMarkerRequested.Broadcast(HitMarkerType);
		}

		UNiagaraSystem* ImpactParticles = PlasterImpactParticles;
		UMetaSoundSource* ImpactSound = PlasterImpactSound;
		UMetaSoundSource* DebrisImpactSound = PlasterDebrisImpactSound;

		switch (const EPhysicalSurface SurfaceHit = UGameplayStatics::GetSurfaceType(HitResultToUse))
		{
		case SurfaceType_Glass:
			ImpactParticles = GlassImpactParticles;
			ImpactSound = GlassImpactSound;
			DebrisImpactSound = GlassDebrisImpactSound;
			break;
		case SurfaceType_Human:
			ImpactParticles = HumanImpactParticles;
			ImpactSound = HumanImpactSound;
			DebrisImpactSound = HumanDebrisImpactSound;
			break;
		case SurfaceType_Plaster: // Default, already assigned
		default:
			break;
		}

		// Play impact sounds
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactPoint);
		UGameplayStatics::PlaySoundAtLocation(this, DebrisImpactSound, ImpactPoint);

		// Spawn particle effects
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactParticles, ImpactPoint);
	}

	// Spawn smoke trail
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BulletTracerVFX, WeaponBarrelLocation, FRotator::ZeroRotator);
}

void UWeaponsComponent::TryPickupWeapon()
{
	if (!CanPickupWeapon())
	{
		return;
	}

	AWeaponPickup* PickupWeapon = WeaponPickupsInRange[0];

	AddWeapon(PickupWeapon->GetWeaponConfig());

	WeaponPickupsInRange.Remove(PickupWeapon);
	PickupWeapon->Destroy();
}

void UWeaponsComponent::AN_NotifyWeaponUnequipped()
{
	if (PendingWeaponType == EWeapon::Unarmed)
	{
		WeaponSwapPhase = EWeaponSwapPhase::None;
	}

	// Equip pending swap weapon
	EquipWeapon(PendingWeaponType);

	// Clear flag
	PendingWeaponType = EWeapon::Unarmed;
}

void UWeaponsComponent::AN_NotifyWeaponEquipped()
{
	WeaponSwapPhase = EWeaponSwapPhase::None;
}

void UWeaponsComponent::UnequipCurrentWeapon()
{
	// Set phase to unequipping
	WeaponSwapPhase = EWeaponSwapPhase::Unequipping;

	// Play unequip animation on old gun
	const FWeapon& PreviousEquippedWeapon = WeaponInventory[EquippedWeaponType];
	OnWeaponAnimationRequested.Broadcast(EquippedWeaponType, nullptr, PreviousEquippedWeapon.Config.CharacterUnequipAnimMontage);
}

void UWeaponsComponent::EquipWeapon(EWeapon WeaponType)
{
	if (WeaponType != EWeapon::Unarmed)
	{
		// Set phase to equipping
		WeaponSwapPhase = EWeaponSwapPhase::Equipping;

		// Play equip animation
		const FWeapon& NewEquippedWeapon = WeaponInventory[WeaponType];
		OnWeaponAnimationRequested.Broadcast(WeaponType, nullptr, NewEquippedWeapon.Config.CharacterEquipAnimMontage);
	}

	// Set new equipped weapon
	SetEquipWeaponType(WeaponType);
}

void UWeaponsComponent::AddWeapon(const FWeaponConfig& WeaponConfig)
{
	FWeapon Weapon;
	Weapon.Config = WeaponConfig;

	Weapon.TotalBullets = WeaponConfig.StartingAmmoCount;

	const int32 StartingAmmoClip = FMath::Min(WeaponConfig.StartingAmmoCount, WeaponConfig.BulletsPerClip);
	Weapon.CurrentBulletsInClip = StartingAmmoClip;

	WeaponInventory.Add(WeaponConfig.WeaponType, Weapon);
	OnWeaponAdded.Broadcast(Weapon);
}

bool UWeaponsComponent::CanPickupWeapon() const
{
	return !WeaponPickupsInRange.IsEmpty();
}

EWeaponFireState UWeaponsComponent::GetWeaponFireState() const
{
	check(EquippedWeaponType != EWeapon::Unarmed);
	const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponType];

	if (EquippedWeapon.bIsReloading)
	{
		return EWeaponFireState::Reloading;
	}

	if (!EquippedWeapon.bFireIntervalElapsed)
	{
		return EWeaponFireState::FireIntervalElapsed;
	}

	const bool bHasAmmoInClip = EquippedWeapon.CurrentBulletsInClip > 0;
	if (!bHasAmmoInClip)
	{
		return EWeaponFireState::NoAmmoInClip;
	}

	return EWeaponFireState::CanFire;
}

USkeletalMeshComponent* UWeaponsComponent::FindWeaponMesh(const FWeaponConfig& Config) const
{
	ensureMsgf(!Config.MeshTag.IsNone(), TEXT("This weapon must set its MeshTag in its config"));

	// Get all Skeletal Mesh Components from our owner
	TArray<USkeletalMeshComponent*> SkeletalMeshComps;
	GetOwner()->GetComponents(USkeletalMeshComponent::StaticClass(), SkeletalMeshComps);

	for (USkeletalMeshComponent* SkeletalMeshComp : SkeletalMeshComps)
	{
		// If any have the matching tag of our equipped weapon
		if (SkeletalMeshComp->ComponentHasTag(Config.MeshTag))
		{
			return SkeletalMeshComp;
		}
	}

	return nullptr;
}

void UWeaponsComponent::AddWeaponPickupInRange(AWeaponPickup* WeaponPickup)
{
	WeaponPickupsInRange.Add(WeaponPickup);

	// TODO: Recalculate closest pickup

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		OnWeaponPickupChanged.Broadcast(CurrentClosestWeaponPickup, WeaponPickup);
		CurrentClosestWeaponPickup = WeaponPickup;
	}
}

void UWeaponsComponent::RemoveWeaponPickupInRange(AWeaponPickup* WeaponPickup)
{
	WeaponPickupsInRange.Remove(WeaponPickup);

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		OnWeaponPickupChanged.Broadcast(CurrentClosestWeaponPickup, nullptr);
		CurrentClosestWeaponPickup = nullptr;
	}
}

void UWeaponsComponent::SetEquipWeaponType(EWeapon WeaponType)
{
	// TODO: Add check for authority 
	EquippedWeaponType = WeaponType;

	const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponType];
	OnWeaponEquipped.Broadcast(EquippedWeapon);
}
