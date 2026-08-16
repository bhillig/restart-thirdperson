// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/RSWeaponHelpers.h"
#include "WeaponsComponent.generated.h"

class UWeaponsComponent;
class AWeaponPickup;
class UNiagaraSystem;
class UMetaSoundSource;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAdded, const FWeapon&, Weapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, const FWeapon&, Weapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipped);

// Used by character classes to play shooting/reloading animations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponAnimationRequested, EWeaponSlot, WeaponSlot, UAnimSequenceBase*, WeaponAnimation, UAnimMontage*, CharacterAnimation);

// Used for weapon pickup UI (only fired for locally controlled pawns)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponPickupChanged, AWeaponPickup*, OldWeaponPickup, AWeaponPickup*, NewWeaponPickup);

// Used for weapon ammo UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChanged, int32, BulletsInClip, int32, TotalBullets);

// Used for hitmarker UI (only fire for locally controlled pawns)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitMarkerRequested, EHitMarkerType, HitMarkerType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API UWeaponsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	UWeaponsComponent();

	/** Register replicated variables */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/** Called when the game begins */
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void TryAddWeapon(const UWeaponDataAsset* WeaponData);

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool HasWeaponEquipped() const { return EquippedWeaponSlot != EWeaponSlot::None; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool HasWeaponInSlot(EWeaponSlot WeaponSlot) const { return FindEntry(WeaponSlot) != nullptr; }

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool HasWeaponInInventory(const UWeaponDataAsset* WeaponData) const;

	const FWeapon* GetEquippedWeapon() const;

	UFUNCTION(BlueprintCallable)
	void TryUnequipWeapon();

	UFUNCTION(BlueprintCallable)
	void TryEquipPrimaryWeapon();

	UFUNCTION(BlueprintCallable)
	void TryEquipSecondaryWeapon();

	UFUNCTION(BlueprintCallable)
	void TryReloadEquippedWeapon();

	UFUNCTION(BlueprintCallable)
	void TryFireWeapon();

	UFUNCTION(BlueprintCallable)
	void TryPickupWeapon();

	UFUNCTION(BlueprintCallable)
	void TryRefillAmmoForWeapon(const UWeaponDataAsset* WeaponData);

	/** Called when the weapon has been unequipped in its unequip animation. Called by an Anim Notify */
	UFUNCTION(BlueprintCallable)
	void AN_NotifyWeaponUnequipped();

	/** Called when the weapon has been unequipped in its unequip animation. Called by an Anim Notify */
	UFUNCTION(BlueprintCallable)
	void AN_NotifyWeaponEquipped();

	// Functions for Weapon Pickups to register
	UFUNCTION(BlueprintCallable)
	void AddWeaponPickupInRange(AWeaponPickup* WeaponPickup);

	UFUNCTION(BlueprintCallable)
	void RemoveWeaponPickupInRange(AWeaponPickup* WeaponPickup);

protected:
	/** Client -> Server request to add a weapon to WeaponInventory */
	UFUNCTION(Server, Reliable)
	void Server_AddWeapon(const UWeaponDataAsset* WeaponData);

	/** Client -> Server request to unequip a weapon */
	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon();

	/** Client -> Server request to equip a primary weapon */
	UFUNCTION(Server, Reliable)
	void Server_EquipPrimaryWeapon();

	/** Client -> Server request to equip a secondary weapon */
	UFUNCTION(Server, Reliable)
	void Server_EquipSecondaryWeapon();

	/** Client -> Server request to reload the equipped weapon */
	UFUNCTION(Server, Reliable)
	void Server_ReloadEquippedWeapon();

	/** Client -> Server request to fire the equipped weapon */
	UFUNCTION(Server, Reliable)
	void Server_FireWeapon();

	/** Client -> Server request to try and pickup a weapon */
	UFUNCTION(Server, Reliable)
	void Server_PickupWeapon();

	/** Client -> Server request to refill ammo for weapon */
	UFUNCTION(Server, Reliable)
	void Server_RefillAmmoForWeapon(const UWeaponDataAsset* WeaponData);

	/** Server -> Owner notify a hit marker type to propagate */
	UFUNCTION(Client, Unreliable)
	void Client_NotifyHitType(EHitMarkerType HitMarkerType);

	/** Server -> All: notify clients to request animations */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_RequestAnimations(EWeaponSlot WeaponSlot, UAnimSequenceBase* WeaponAnimation, UAnimMontage* CharacterAnimation);

	/** Server -> All: notify clients to play a sound at a location */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlaySoundAtLocation(UMetaSoundSource* Sound, FVector Location);

	/** Server -> All: notify clients to spawn a niagara system at a location */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnSystemAtLocation(UNiagaraSystem* System, FVector Location);

protected:
	/** Only modifier to WeaponInventory. Called on server */
	void AddWeapon(const UWeaponDataAsset* WeaponData);

	/** Only modifier to unequip a weapon. Called on server */
	void UnequipWeapon();

	/** Only modifier to equip the primary weapon. Called on server */
	void EquipPrimaryWeapon();

	/** Only modifier to equip the secondary weapon. Called on server */
	void EquipSecondaryWeapon();

	/** Only seam to fire a weapon. Called on server */
	void FireWeapon();

	/** Only seam to try and pickup a weapon. Called on server */
	void PickupWeapon();

	/** Handles broadcasting delegates when EquippedWeaponSlot changes. Called by rep notify and server */
	void HandleEquippedWeaponSlotChanged();

	/** Handles rebuilding the cache and broadcasting delegates when WeaponInventory changes. Called by rep notify and server */
	void HandleWeaponInventoryChanged(const TArray<FWeaponSlotEntry>& OldInventory);

	/** Returns whether the primary weapon (if it exists) can be equipped */
	bool CanEquipPrimaryWeapon() const;

	/** Returns whether the secondary weapon (if it exists) can be equipped */
	bool CanEquipSecondaryWeapon() const;

	/** Helper function that performs basic checks for whether swapping is blocked */
	bool IsSwappingBlocked() const;

	/** Equips new weapon (and unequips old if relevant). Called by public TryEquip functions */
	void EquipWeaponSlot(EWeaponSlot WeaponSlot);

	/** Only setter for equipped weapon slot. Called on server */
	void SetEquipWeaponSlot(EWeaponSlot WeaponSlot);

	void EquipWeapon(EWeaponSlot WeaponSlot);

	/** Returns whether we can reload the currently equipped weapon. False if nothing is equipped */
	bool CanReloadEquippedWeapon() const;

	/** Reloads the currently equipped weapon */
	void ReloadEquippedWeapon();

	/** Callback for when the equip timer completes */
	void OnEquipComplete();

	/** Callback for when the unequip timer completes */
	void OnUnequipComplete();

public:
	/** On Weapon Added Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAdded OnWeaponAdded;

	/** On Weapon Equipped Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponEquipped OnWeaponEquipped;

	/** Weapon Unequipped Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponUnequipped OnWeaponUnequipped;

	/** Weapon Animation Requested Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAnimationRequested OnWeaponAnimationRequested;

	/** Weapon Ammo Changed Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAmmoChanged OnWeaponAmmoChanged;

	/** Weapon Pickup Changed Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponPickupChanged OnWeaponPickupChanged;

	/** Hit Marker Requested Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnHitMarkerRequested OnHitMarkerRequested;

protected:
	/** VFX Begin */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UMetaSoundSource> PlasterImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UMetaSoundSource> PlasterDebrisImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UMetaSoundSource> GlassImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UMetaSoundSource> GlassDebrisImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UMetaSoundSource> HumanImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UMetaSoundSource> HumanDebrisImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> PlasterImpactParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> GlassImpactParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> HumanImpactParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> BulletTracerVFX;
	/** VFX End */
private:

	bool CanPickupWeapon() const;

	EWeaponFireState GetWeaponFireState() const;

	USkeletalMeshComponent* FindWeaponMesh(const FWeaponConfig& Config) const;

	FWeaponSlotEntry* FindMutableEntry(EWeaponSlot WeaponSlot);

	const FWeaponSlotEntry* FindEntry(EWeaponSlot WeaponSlot) const;

protected:
	/** Weapon slot currently equipped */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeaponSlot)
	EWeaponSlot EquippedWeaponSlot = EWeaponSlot::None;

	/** Weapon inventory. Should have a maximum of one of each weapon slot type */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponInventory)
	TArray<FWeaponSlotEntry> WeaponInventory;

	/** Weapon type requested. Cached while we unequip the current weapon */
	UPROPERTY(Replicated)
	EWeaponSlot PendingWeaponSlot = EWeaponSlot::None;

	/** Swap state of unequipping/equipping weapons. None when not */
	UPROPERTY(Replicated)
	EWeaponSwapPhase WeaponSwapPhase = EWeaponSwapPhase::None;

private:
	/** Rep notify for EquippedWeaponSlot */
	UFUNCTION()
	void OnRep_EquippedWeaponSlot();

	/** Rep notify for WeaponInventory */
	UFUNCTION()
	void OnRep_WeaponInventory(const TArray<FWeaponSlotEntry>& OldWeaponInventory);

	/** Interface for retrieving weapon's aim ray (character, enemy, etc) */
	TScriptInterface<IWeaponAimSource> WeaponAimSource;

	/** Weapons in range to be picked up */
	TArray<TObjectPtr<AWeaponPickup>> WeaponPickupsInRange;

	/** Closest weapon pickup in range */
	TObjectPtr<AWeaponPickup> CurrentClosestWeaponPickup;

	/** Timer handle for managing when we equip a weapon */
	FTimerHandle TimerHandle_Equip;

	/** Timer handle for managing when we unequip a weapon */
	FTimerHandle TimerHandle_Unequip;
};
