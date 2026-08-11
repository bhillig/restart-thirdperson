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

	if (AActor* Owner = GetOwner())
	{
		IWeaponAimSource* AimSource = Cast<IWeaponAimSource>(Owner);
		WeaponAimSource.SetObject(Owner);
		WeaponAimSource.SetInterface(AimSource);
		ensureMsgf(WeaponAimSource, TEXT("%s requires its owner to implement IWeaponAimSource"), *GetName());
	}
}

void UWeaponsComponent::TryUnequipWeapon()
{
	if (!IsSwappingBlocked())
	{
		EquipWeaponSlot(EWeaponSlot::None);
	}
}

void UWeaponsComponent::TryEquipPrimaryWeapon()
{
	if (CanEquipPrimaryWeapon())
	{
		EquipWeaponSlot(EWeaponSlot::Primary);
	}
}

void UWeaponsComponent::TryEquipSecondaryWeapon()
{
	if (CanEquipSecondaryWeapon())
	{
		EquipWeaponSlot(EWeaponSlot::Secondary);
	}
}

void UWeaponsComponent::TryReloadEquippedWeapon()
{
	if (CanReloadEquippedWeapon())
	{
		ReloadEquippedWeapon();
	}
}

void UWeaponsComponent::EquipWeaponSlot(EWeaponSlot WeaponSlot)
{
	// Check if we need to unequip first
	if (HasWeaponEquipped())
	{
		PlayUnequipAnimation();

		// Set pending weapon to equip
		PendingWeaponSlot = WeaponSlot;
		return;
	}

	EquipWeapon(WeaponSlot);
}

bool UWeaponsComponent::CanReloadEquippedWeapon() const
{
	if (!HasWeaponEquipped())
	{
		return false;
	}

	if (WeaponSwapPhase != EWeaponSwapPhase::None)
	{
		return false;
	}

	const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponSlot];
	if (EquippedWeapon.bIsReloading)
	{
		return false;
	}

	// For each weapon we can reload if we have bullets outside our current clip and our current clip isn't full
	return EquippedWeapon.TotalBullets > EquippedWeapon.CurrentBulletsInClip && EquippedWeapon.CurrentBulletsInClip != EquippedWeapon.Data->Config.BulletsPerClip;
}

void UWeaponsComponent::ReloadEquippedWeapon()
{
	FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponSlot];
	const FWeaponConfig& WeaponConfig = EquippedWeapon.Data->Config;
	const EWeaponSlot WeaponSlot = WeaponConfig.WeaponSlot;

	EquippedWeapon.bIsReloading = true;

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, WeaponSlot]()
		{
			if (FWeapon* Weapon = WeaponInventory.Find(WeaponSlot))
			{
				Weapon->bIsReloading = false;
				// Take as much ammo from remaining bullets as possible (up to clip amount)
				const int32 BulletsOutsideClip = Weapon->TotalBullets - Weapon->CurrentBulletsInClip;
				const int32 BulletsInNewClip = FMath::Min(Weapon->Data->Config.BulletsPerClip - Weapon->CurrentBulletsInClip, BulletsOutsideClip);
				Weapon->CurrentBulletsInClip += BulletsInNewClip;
				OnWeaponAmmoChanged.Broadcast(Weapon->CurrentBulletsInClip, Weapon->TotalBullets);
			}
		});


	GetOwner()->GetWorldTimerManager().SetTimer(EquippedWeapon.TimerHandle_Reload, Delegate, WeaponConfig.ReloadDuration, false);

	OnWeaponAnimationRequested.Broadcast(EquippedWeaponSlot, WeaponConfig.ReloadAnim, WeaponConfig.CharacterReloadAnimMontage);
}

void UWeaponsComponent::TryFireWeapon()
{
	if (!HasWeaponEquipped())
	{
		return;
	}

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

	FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponSlot];
	const FWeaponConfig& WeaponConfig = EquippedWeapon.Data->Config;
	const EWeaponSlot WeaponSlot = WeaponConfig.WeaponSlot;

	EquippedWeapon.bFireIntervalElapsed = false;

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, WeaponSlot]()
		{
			if (FWeapon* Weapon = WeaponInventory.Find(WeaponSlot))
			{
				Weapon->bFireIntervalElapsed = true;
			}
		});

	const USkeletalMeshComponent* WeaponMesh = FindWeaponMesh(WeaponConfig);
	check(WeaponMesh);

	const FVector WeaponBarrelLocation = WeaponMesh->GetBoneLocation(WeaponConfig.MuzzleSocketName);

	GetOwner()->GetWorldTimerManager().SetTimer(EquippedWeapon.TimerHandle_FireInterval, Delegate, WeaponConfig.FireInterval, false);

	OnWeaponAnimationRequested.Broadcast(EquippedWeaponSlot, WeaponConfig.FireAnim, WeaponConfig.CharacterFireAnimMontage);

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

	AddWeapon(PickupWeapon->GetWeaponData());

	WeaponPickupsInRange.Remove(PickupWeapon);
	PickupWeapon->Destroy();
}

void UWeaponsComponent::AN_NotifyWeaponUnequipped()
{
	if (PendingWeaponSlot == EWeaponSlot::None)
	{
		WeaponSwapPhase = EWeaponSwapPhase::None;
	}

	// Equip pending swap weapon
	EquipWeapon(PendingWeaponSlot);

	// Clear flag
	PendingWeaponSlot = EWeaponSlot::None;
}

void UWeaponsComponent::AN_NotifyWeaponEquipped()
{
	WeaponSwapPhase = EWeaponSwapPhase::None;
}

void UWeaponsComponent::PlayUnequipAnimation()
{
	// Set phase to unequipping
	WeaponSwapPhase = EWeaponSwapPhase::Unequipping;

	// Play unequip animation on old gun
	const FWeaponConfig& PreviousEquippedWeaponConfig = WeaponInventory[EquippedWeaponSlot].Data->Config;
	OnWeaponAnimationRequested.Broadcast(EquippedWeaponSlot, nullptr, PreviousEquippedWeaponConfig.CharacterUnequipAnimMontage);
}

void UWeaponsComponent::EquipWeapon(EWeaponSlot WeaponSlot)
{
	if (WeaponSlot != EWeaponSlot::None)
	{
		// Set phase to equipping
		WeaponSwapPhase = EWeaponSwapPhase::Equipping;

		// Play equip animation
		const FWeaponConfig& NewEquippedWeaponConfig = WeaponInventory[WeaponSlot].Data->Config;
		OnWeaponAnimationRequested.Broadcast(WeaponSlot, nullptr, NewEquippedWeaponConfig.CharacterEquipAnimMontage);
	}

	// Set new equipped weapon
	SetEquipWeaponSlot(WeaponSlot);
}

void UWeaponsComponent::AddWeapon(const UWeaponDataAsset* WeaponData)
{
	FWeapon Weapon;
	Weapon.Data = WeaponData;

	const FWeaponConfig& WeaponConfig = Weapon.Data->Config;

	Weapon.TotalBullets = WeaponConfig.StartingAmmoCount;

	const int32 StartingAmmoClip = FMath::Min(WeaponConfig.StartingAmmoCount, WeaponConfig.BulletsPerClip);
	Weapon.CurrentBulletsInClip = StartingAmmoClip;

	WeaponInventory.Add(WeaponConfig.WeaponSlot, Weapon);
	OnWeaponAdded.Broadcast(Weapon);
}

bool UWeaponsComponent::CanPickupWeapon() const
{
	return !WeaponPickupsInRange.IsEmpty();
}

EWeaponFireState UWeaponsComponent::GetWeaponFireState() const
{
	check(EquippedWeaponSlot != EWeaponSlot::None);
	const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponSlot];

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
bool UWeaponsComponent::CanEquipPrimaryWeapon() const
{
	if (IsSwappingBlocked())
	{
		return false;
	}

	if (EquippedWeaponSlot == EWeaponSlot::Primary || !WeaponInventory.Contains(EWeaponSlot::Primary))
	{
		return false;
	}

	return true;
}

bool UWeaponsComponent::CanEquipSecondaryWeapon() const
{
	if (IsSwappingBlocked())
	{
		return false;
	}

	if (EquippedWeaponSlot == EWeaponSlot::Secondary || !WeaponInventory.Contains(EWeaponSlot::Secondary))
	{
		return false;
	}

	return true;
}

bool UWeaponsComponent::IsSwappingBlocked() const
{
	if (WeaponInventory.Contains(EquippedWeaponSlot) && WeaponInventory[EquippedWeaponSlot].bIsReloading)
	{
		// We are currently reloading
		return true;
	}

	if (WeaponSwapPhase != EWeaponSwapPhase::None)
	{
		// We are currently swapping
		return true;
	}

	return false;
}

void UWeaponsComponent::SetEquipWeaponSlot(EWeaponSlot WeaponSlot)
{
	// TODO: Add check for authority 
	EquippedWeaponSlot = WeaponSlot;

	if (HasWeaponEquipped())
	{
		const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponSlot];
		OnWeaponEquipped.Broadcast(EquippedWeapon);
	}
	else
	{
		OnWeaponUnequipped.Broadcast();
	}
}
