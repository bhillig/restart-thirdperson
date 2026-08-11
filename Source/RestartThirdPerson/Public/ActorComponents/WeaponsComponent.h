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

	/** On Weapon Added Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAdded OnWeaponAdded;

	/** On Weapon Equipped Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponEquipped OnWeaponEquipped;

	/** Weapon Unequipped Delegate */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponUnequipped OnWeaponUnequipped;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAnimationRequested OnWeaponAnimationRequested;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponAmmoChanged OnWeaponAmmoChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponPickupChanged OnWeaponPickupChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHitMarkerRequested OnHitMarkerRequested;

protected:
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void AddWeapon(const UWeaponDataAsset* WeaponData);

	UFUNCTION(BlueprintCallable)
	bool HasWeaponEquipped() const { return EquippedWeaponSlot != EWeaponSlot::None; }

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

	/** Plays the unequip animation on the current weapon */
	void PlayUnequipAnimation();

	void EquipWeapon(EWeaponSlot WeaponSlot);

	/** Returns whether we can reload the currently equipped weapon. False if nothing is equipped */
	bool CanReloadEquippedWeapon() const;

	/** Reloads the currently equipped weapon */
	void ReloadEquippedWeapon();

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

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponSlot EquippedWeaponSlot = EWeaponSlot::None;

	/** Weapon type requested. Cached while we unequip the current weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponSlot PendingWeaponSlot = EWeaponSlot::None;

	/** Swap state of unequipping/equipping weapons. None when not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponSwapPhase WeaponSwapPhase = EWeaponSwapPhase::None;

private:
	TMap<EWeaponSlot, FWeapon> WeaponInventory;

	TScriptInterface<IWeaponAimSource> WeaponAimSource; // Interface for retrieving weapon's aim ray (character, enemy, etc)

	TArray<TObjectPtr<AWeaponPickup>> WeaponPickupsInRange; // Weapons in range to be picked up

	TObjectPtr<AWeaponPickup> CurrentClosestWeaponPickup; // Closest weapon pickup in range
};
