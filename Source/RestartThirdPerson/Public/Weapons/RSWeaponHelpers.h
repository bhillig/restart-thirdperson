// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSWeaponHelpers.generated.h"

class UMetaSoundSource;
class UWeaponsComponent;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ShowOnlyInnerProperties))
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