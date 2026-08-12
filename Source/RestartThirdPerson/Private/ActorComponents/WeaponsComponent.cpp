// Brandon Hillig 2026

#include "ActorComponents/WeaponsComponent.h"

#include "MetasoundSource.h"
#include "NiagaraFunctionLibrary.h"
#include "ActorComponents/AttributesComponent.h"
#include "Actors/WeaponPickup.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

UWeaponsComponent::UWeaponsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UWeaponsComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeaponSlot);
	DOREPLIFETIME(ThisClass, WeaponInventory);
	DOREPLIFETIME(ThisClass, PendingWeaponSlot);
	DOREPLIFETIME(ThisClass, WeaponSwapPhase);
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

void UWeaponsComponent::TryAddWeapon(const UWeaponDataAsset* WeaponData)
{
	if (GetOwner()->HasAuthority())
	{
		// If we are on the server
		AddWeapon(WeaponData);
		return;
	}

	// Request the server to add the weapon
	Server_AddWeapon(WeaponData);
}

void UWeaponsComponent::TryUnequipWeapon()
{
	// Local early out. We still perform checks on the server
	if (IsSwappingBlocked())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		// Call the authoritative function
		UnequipWeapon();
		return;
	}

	// Request the server to unequip the weapon
	Server_UnequipWeapon();
}

void UWeaponsComponent::TryEquipPrimaryWeapon()
{
	// Local early out. We still perform checks on the server
	if (!CanEquipPrimaryWeapon())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		// Call the authoritative function
		EquipPrimaryWeapon();
		return;
	}

	// Request the server to equip the primary weapon
	Server_EquipPrimaryWeapon();
}

void UWeaponsComponent::TryEquipSecondaryWeapon()
{
	// Local early out. We still perform checks on the server
	if (!CanEquipSecondaryWeapon())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		// Call the authoritative function
		EquipSecondaryWeapon();
		return;
	}

	// Request the server to equip the secondary weapon
	Server_EquipSecondaryWeapon();
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
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	// Check if we need to unequip first
	if (HasWeaponEquipped())
	{
		// Set phase to unequipping
		WeaponSwapPhase = EWeaponSwapPhase::Unequipping;

		const FWeaponConfig& PreviousEquippedWeaponConfig = WeaponCache[EquippedWeaponSlot].Data->Config;

		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Unequip, this, &UWeaponsComponent::OnUnequipComplete, PreviousEquippedWeaponConfig.UnequipDuration);

		// Request unequip animation on old gun
		Multicast_RequestAnimations(EquippedWeaponSlot, nullptr, PreviousEquippedWeaponConfig.CharacterUnequipAnimMontage);

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

	const FWeapon& EquippedWeapon = WeaponCache[EquippedWeaponSlot];
	if (EquippedWeapon.bIsReloading)
	{
		return false;
	}

	// For each weapon we can reload if we have bullets outside our current clip and our current clip isn't full
	return EquippedWeapon.TotalBullets > EquippedWeapon.CurrentBulletsInClip && EquippedWeapon.CurrentBulletsInClip != EquippedWeapon.Data->Config.BulletsPerClip;
}

void UWeaponsComponent::ReloadEquippedWeapon()
{
	FWeapon& EquippedWeapon = WeaponCache[EquippedWeaponSlot];
	const FWeaponConfig& WeaponConfig = EquippedWeapon.Data->Config;
	const EWeaponSlot WeaponSlot = WeaponConfig.WeaponSlot;

	EquippedWeapon.bIsReloading = true;

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, WeaponSlot]()
		{
			if (FWeapon* Weapon = WeaponCache.Find(WeaponSlot))
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

void UWeaponsComponent::OnEquipComplete()
{
	WeaponSwapPhase = EWeaponSwapPhase::None;
}

void UWeaponsComponent::OnUnequipComplete()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on server
	if (PendingWeaponSlot == EWeaponSlot::None)
	{
		WeaponSwapPhase = EWeaponSwapPhase::None;
	}

	// Equip pending swap weapon
	EquipWeapon(PendingWeaponSlot);

	// Clear flag
	PendingWeaponSlot = EWeaponSlot::None;
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

	FWeapon& EquippedWeapon = WeaponCache[EquippedWeaponSlot];
	const FWeaponConfig& WeaponConfig = EquippedWeapon.Data->Config;
	const EWeaponSlot WeaponSlot = WeaponConfig.WeaponSlot;

	EquippedWeapon.bFireIntervalElapsed = false;

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, WeaponSlot]()
		{
			if (FWeapon* Weapon = WeaponCache.Find(WeaponSlot))
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

	// TODO: Run on server
	AWeaponPickup* PickupWeapon = WeaponPickupsInRange[0];

	AddWeapon(PickupWeapon->GetWeaponData());

	WeaponPickupsInRange.Remove(PickupWeapon);
	PickupWeapon->Destroy();
}

void UWeaponsComponent::AN_NotifyWeaponUnequipped()
{
	// TODO: Can perform weapon detachment logic here
}

void UWeaponsComponent::AN_NotifyWeaponEquipped()
{
	// TODO: Can perform weapon attachment logic here
}

void UWeaponsComponent::EquipWeapon(EWeaponSlot WeaponSlot)
{
	if (WeaponSlot != EWeaponSlot::None)
	{
		// Set phase to equipping
		WeaponSwapPhase = EWeaponSwapPhase::Equipping;

		const FWeaponConfig& NewEquippedWeaponConfig = WeaponCache[WeaponSlot].Data->Config;

		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Equip, this, &UWeaponsComponent::OnEquipComplete, NewEquippedWeaponConfig.EquipDuration);

		// Request equip animation
		Multicast_RequestAnimations(WeaponSlot, nullptr, NewEquippedWeaponConfig.CharacterEquipAnimMontage);
	}

	// Set new equipped weapon
	SetEquipWeaponSlot(WeaponSlot);
}

void UWeaponsComponent::Multicast_RequestAnimations_Implementation(EWeaponSlot WeaponSlot, UAnimSequenceBase* WeaponAnimation, UAnimMontage* CharacterAnimation)
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	OnWeaponAnimationRequested.Broadcast(WeaponSlot, WeaponAnimation, CharacterAnimation);
}

void UWeaponsComponent::Server_EquipPrimaryWeapon_Implementation()
{
	EquipPrimaryWeapon();
}

void UWeaponsComponent::Server_EquipSecondaryWeapon_Implementation()
{
	EquipSecondaryWeapon();
}

void UWeaponsComponent::AddWeapon(const UWeaponDataAsset* WeaponData)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Only called on server
	FWeapon Weapon;
	Weapon.Data = WeaponData;

	const FWeaponConfig& WeaponConfig = Weapon.Data->Config;

	Weapon.TotalBullets = WeaponConfig.StartingAmmoCount;

	const int32 StartingAmmoClip = FMath::Min(WeaponConfig.StartingAmmoCount, WeaponConfig.BulletsPerClip);
	Weapon.CurrentBulletsInClip = StartingAmmoClip;

	// Capture old weapon inventory
	TArray<FWeaponSlotEntry> OldWeaponInventory = WeaponInventory;

	WeaponInventory.Add(FWeaponSlotEntry{ .Weapon = Weapon, .Slot = WeaponConfig.WeaponSlot });

	HandleWeaponInventoryChanged(OldWeaponInventory);
}

void UWeaponsComponent::UnequipWeapon()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	if (IsSwappingBlocked())
	{
		return;
	}

	EquipWeaponSlot(EWeaponSlot::None);
}

void UWeaponsComponent::EquipPrimaryWeapon()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	if (!CanEquipPrimaryWeapon())
	{
		return;
	}

	EquipWeaponSlot(EWeaponSlot::Primary);
}

void UWeaponsComponent::EquipSecondaryWeapon()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	if (!CanEquipSecondaryWeapon())
	{
		return;
	}

	EquipWeaponSlot(EWeaponSlot::Secondary);
}

void UWeaponsComponent::HandleEquippedWeaponSlotChanged()
{
	if (HasWeaponEquipped())
	{
		const FWeapon& EquippedWeapon = WeaponCache[EquippedWeaponSlot];
		OnWeaponEquipped.Broadcast(EquippedWeapon);
	}
	else
	{
		OnWeaponUnequipped.Broadcast();
	}
}

bool UWeaponsComponent::CanPickupWeapon() const
{
	return !WeaponPickupsInRange.IsEmpty();
}

EWeaponFireState UWeaponsComponent::GetWeaponFireState() const
{
	check(EquippedWeaponSlot != EWeaponSlot::None);
	const FWeapon& EquippedWeapon = WeaponCache[EquippedWeaponSlot];

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

void UWeaponsComponent::OnRep_EquippedWeaponSlot()
{
	HandleEquippedWeaponSlotChanged();
}

void UWeaponsComponent::OnRep_WeaponInventory(const TArray<FWeaponSlotEntry>& OldWeaponInventory)
{
	HandleWeaponInventoryChanged(OldWeaponInventory);
}

void UWeaponsComponent::HandleWeaponInventoryChanged(const TArray<FWeaponSlotEntry>& OldWeaponInventory)
{
	// Rebuild the cache
	WeaponCache.Reset();
	for (const auto& WeaponEntry : WeaponInventory)
	{
		WeaponCache.Add(WeaponEntry.Slot, WeaponEntry.Weapon);
	}

	// Determine what weapons were added/removed
	TArray<FWeaponSlotEntry> Added = WeaponInventory.FilterByPredicate(
		[&OldWeaponInventory](const FWeaponSlotEntry& Entry)
		{
			return !OldWeaponInventory.Contains(Entry);
		});
	TArray<FWeaponSlotEntry> Removed = OldWeaponInventory.FilterByPredicate(
		[this](const FWeaponSlotEntry& Entry)
		{
			return !WeaponInventory.Contains(Entry);
		});

	for (const auto& WeaponEntry : Added)
	{
		OnWeaponAdded.Broadcast(WeaponEntry.Weapon);
	}

	// TODO: Broadcast weapons removed
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

void UWeaponsComponent::Server_UnequipWeapon_Implementation()
{
	UnequipWeapon();
}

void UWeaponsComponent::Server_AddWeapon_Implementation(const UWeaponDataAsset* WeaponData)
{
	AddWeapon(WeaponData);
}

bool UWeaponsComponent::CanEquipPrimaryWeapon() const
{
	if (IsSwappingBlocked())
	{
		return false;
	}

	if (EquippedWeaponSlot == EWeaponSlot::Primary || !WeaponCache.Contains(EWeaponSlot::Primary))
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

	if (EquippedWeaponSlot == EWeaponSlot::Secondary || !WeaponCache.Contains(EWeaponSlot::Secondary))
	{
		return false;
	}

	return true;
}

bool UWeaponsComponent::IsSwappingBlocked() const
{
	if (WeaponCache.Contains(EquippedWeaponSlot) && WeaponCache[EquippedWeaponSlot].bIsReloading)
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
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	EquippedWeaponSlot = WeaponSlot;

	HandleEquippedWeaponSlotChanged();
}
