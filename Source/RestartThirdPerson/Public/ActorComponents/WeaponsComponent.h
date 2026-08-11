// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponsComponent.generated.h"

class UWeaponsComponent;
class AWeaponPickup;
class UNiagaraSystem;
class UMetaSoundSource;

UINTERFACE(MinimalAPI)
class UWeaponAimSource : public UInterface { GENERATED_BODY() };

class RESTARTTHIRDPERSON_API IWeaponAimSource
{
	GENERATED_BODY()
public:
	// Returns world-space origin + unit direction for the next shot.
	virtual void GetWeaponAimRay(FVector& OutOrigin, FVector& OutDirection) const = 0;

	// Returns the weapons component
	virtual UWeaponsComponent* GetWeaponsComponent() const = 0;
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	None = 0,
	Primary = 1,
	Secondary = 2,
};

UENUM(BlueprintType)
enum class EWeaponSwapPhase : uint8
{
	None = 0,
	Unequipping = 1,
	Equipping = 2
};

USTRUCT(BlueprintType)
struct FWeaponConfig
{
	GENERATED_BODY()

	// Weapon Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FText WeaponName;

	// Weapon Slot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponSlot WeaponSlot = EWeaponSlot::None;

	// Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMesh> Mesh;

	// Mesh Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MeshTag;

	// Muzzle Socket Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName = "Barrel";

	// Character Animation When Equipping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> CharacterEquipAnimMontage;

	// Character Animation When Unequipping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> CharacterUnequipAnimMontage;

	// Character Animation When Firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> CharacterFireAnimMontage;

	// Weapon Animation When Firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimSequence> FireAnim;

	// Character Animation When Reloading
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> CharacterReloadAnimMontage;

	// Weapon Animation When Reloading
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimSequence> ReloadAnim;

	// Bullets Per Clip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 BulletsPerClip;

	// Damage Per Bullet
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DamagePerBullet;

	// Reload Duration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ReloadDuration;

	// Fire Interval
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireInterval;

	// Fire Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UMetaSoundSource> FireSound;

	// Mesh Icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UMaterialInstance> MeshIcon;

	// Bullet Icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UMaterialInstance> BulletIcon;

	// Starting Ammo Count
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 StartingAmmoCount;
};

UCLASS(BlueprintType)
class RESTARTTHIRDPERSON_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta=(ShowOnlyInnerProperties))
	FWeaponConfig Config;
};

USTRUCT(BlueprintType)
struct FWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<const UWeaponDataAsset> Data;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	int32 CurrentBulletsInClip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 TotalBullets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bFireIntervalElapsed = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	FTimerHandle TimerHandle_FireInterval; // Timer responsible for using the current weapon's fire interval to determine when bFireIntervalElapsed is set back to true

	FTimerHandle TimerHandle_Reload; // Timer responsible for using the current weapon's reload duration to determine when bIsReloading is set back to false
};

UENUM(BlueprintType)
enum class EWeaponFireState : uint8
{
	NoAmmoInClip = 0,
	Reloading = 1,
	FireIntervalElapsed = 2,
	CanFire = 3
};

UENUM(BlueprintType)
enum class EHitMarkerType : uint8
{
	Base = 0,
	Kill = 1
};

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
