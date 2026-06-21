// Brandon Hillig 2026

#include "ActorComponents/WeaponsComponent.h"

#include "MetasoundSource.h"
#include "NiagaraFunctionLibrary.h"
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

	if (WeaponType == EWeapon::Unarmed)
	{
		return true;
	}

	return WeaponInventory.Contains(WeaponType);
}

void UWeaponsComponent::EquipWeaponType(EWeapon WeaponType)
{
	EquippedWeaponType = WeaponType;

	const FWeapon& EquippedWeapon = WeaponInventory[EquippedWeaponType];
	OnWeaponEquipped.Broadcast(EquippedWeapon);
}

bool UWeaponsComponent::CanReloadEquippedWeapon() const
{
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
	if (WeaponFireState != EWeaponFireState::CanFire)
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
		const FHitResult& HitResultToUse = CameraHitResult.bBlockingHit ? CameraHitResult : GunBarrelToImpactPointHitResult;
		AActor* HitActor = HitResultToUse.GetActor();

		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		AController* EventInstigator = InstigatorPawn ? InstigatorPawn->GetController() : nullptr;

		// Apply damage
		UGameplayStatics::ApplyPointDamage(HitActor, WeaponConfig.DamagePerBullet, WeaponDirection, HitResultToUse, EventInstigator, GetOwner(), UDamageType::StaticClass());

		UNiagaraSystem* ImpactParticles = PlasterImpactParticles;
		UMetaSoundSource* ImpactSound = PlasterImpactSound;
		UMetaSoundSource* DebrisImpactSound = PlasterDebrisImpactSound;

		const EPhysicalSurface SurfaceHit = UGameplayStatics::GetSurfaceType(HitResultToUse);
		switch (SurfaceHit)
		{
		case SurfaceType1: // Glass
			ImpactParticles = GlassImpactParticles;
			ImpactSound = GlassImpactSound;
			DebrisImpactSound = GlassDebrisImpactSound;
			break;
		case SurfaceType_Default: // Default, already assigned
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
	rs::LogOnce(FString::Printf(TEXT("In Range!")), FColor::Emerald, 2.0f);
}

void UWeaponsComponent::RemoveWeaponPickupInRange(AWeaponPickup* WeaponPickup)
{
	WeaponPickupsInRange.Remove(WeaponPickup);
	rs::LogOnce(FString::Printf(TEXT("Out Of Range!")), FColor::Emerald, 2.0f);
}