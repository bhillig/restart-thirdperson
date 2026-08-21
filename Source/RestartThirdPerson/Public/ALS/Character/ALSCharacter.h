// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponsComponent.h"
#include "GameFramework/Character.h"
#include "ALSCharacter.generated.h"

class URSPlayerVoiceComponent;
class URSInteractComponent;
class URSDamageFeedbackComponent;
class UWeaponsComponent;
class UAttributesComponent;
class UNiagaraSystem;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class UMetaSoundSource;

USTRUCT(BlueprintType)
struct FWeaponSocketLocations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName PrimaryEquipped = "Crossbow_Hand_Socket";

	UPROPERTY(EditAnywhere)
	FName SecondaryEquipped = "Pistol_hand_Socket";

	UPROPERTY(EditAnywhere)
	FName PrimaryUnequipped = "RifleHolster";

	UPROPERTY(EditAnywhere)
	FName SecondaryUnequipped = "Holster_Pistol";
};


UENUM(BlueprintType)
enum class EGate : uint8
{
	Walking = 0,
	Jogging = 1,
	Crouching = 2
};

USTRUCT(BlueprintType)
struct FGateSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float MaxWalkSpeed;
	
	UPROPERTY(EditAnywhere)
	float MaxAcceleration;

	UPROPERTY(EditAnywhere)
	float BrakingDeceleration;

	UPROPERTY(EditAnywhere)
	float BrakingFrictionFactor;

	UPROPERTY(EditAnywhere)
	float BrakingFriction;

	UPROPERTY(EditAnywhere)
	bool bUseSeparateBrakingFriction;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGateSwitched, EGate, Gate);

UCLASS()
class RESTARTTHIRDPERSON_API AALSCharacter : public ACharacter, public IWeaponAimSource
{
	GENERATED_BODY()

public:
	/** Constructor */
	AALSCharacter();

	/** Register replicated variables */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
	FOnGateSwitched OnGateSwitched;

public:

	// Returns Distance From Ground, 0.f if on ground, -1.f if not above ground.
	float GetDistanceFromGround() const { return DistanceFromGround; }

	/** Returns whether this player is dead */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool IsDead() const { return bIsDead; }

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** IWeaponAimSource Begin */
	virtual void GetWeaponAimRay(FVector& OutOrigin, FVector& OutDirection) const override;

	virtual UWeaponsComponent* GetWeaponsComponent() const override;
	/** IWeaponAimSource End */

protected:

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URSInteractComponent> InteractComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAttributesComponent> AttributesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWeaponsComponent> WeaponsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URSDamageFeedbackComponent> DamageFeedbackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URSPlayerVoiceComponent> VoiceComponent;

	// Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> ALSInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> StruggleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleCrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireWeaponAction;;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UnequipWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipPrimaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipSecondaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipScrollAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleSlowMotionAction;

	// Character Movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	TMap<EGate, FGateSettings> GateSettingsMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float DistanceFromGround;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float AimingSpringArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float JoggingSpringArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float AimZoomInSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float AimZoomOutSpeed;

	// Weapon

	/** Starting Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> StartingWeaponData;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FWeaponSocketLocations WeaponSocketLocations;

protected:

	// Anim Instances
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Instance")
	TSubclassOf<UAnimInstance> BaseLayersAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Instance")
	TSubclassOf<UAnimInstance> UnarmedAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Instance")
	TSubclassOf<UAnimInstance> PistolAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim Instance")
	TSubclassOf<UAnimInstance> RifleAnimInstance;

	/** Animations */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

private:

	void OnMoveTriggered(const FInputActionValue& Value);

	void OnLookTriggered(const FInputActionValue& Value);

	void OnInteractStarted();

	void OnAimStarted();

	void OnAimCompleted();

	void OnCrouchToggled();

	void OnFireStarted();

	void OnFireCompleted();

	void OnWeaponReloadStarted();

	void SwitchGate(EGate Gate);

	void OnUnequipWeaponPressed();

	void OnPrimaryWeaponEquippedPressed();

	void OnSecondaryWeaponEquippedPressed();

	void OnEquipScrollTriggered(const FInputActionValue& Value);

	void ToggleSlowMotion();

	UFUNCTION()
	void OnWeaponAdded(const FWeapon& Weapon);

	UFUNCTION()
	void OnWeaponEquipped(const FWeapon& Weapon);

	UFUNCTION()
	void OnWeaponUnequipped();

	UFUNCTION()
	void OnWeaponAnimationsRequested(EWeaponSlot WeaponSlot, UAnimSequenceBase* WeaponAnimation, UAnimMontage* CharacterAnimation);

	void RefreshWeaponVisuals();

	void UpdateMeshLocationForWeapon(EWeaponSlot WeaponSlot);

	void DetachAllWeaponMeshes();

	void UpdateAnimInstanceForWeapon(EWeaponSlot WeaponSlot);

	void UpdateAnimInstanceForUnarmed();

	FName GetUnequippedSocketName(EWeaponSlot WeaponSlot) const;

	/** Called when the health attribute changes */
	UFUNCTION()
	void OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser);

	/** Called when the health attribute reaches zero */
	UFUNCTION()
	void OnDeath(AController* EventInstigator, AActor* DamageCauser);

	/** Handles death logic for all machines. Called on server and clients */
	void HandleDeath();

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGate CurrentGate;

	float CurrentSpringArmLength;

	/** State of whether the player is dead */
	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead = false;

protected:
	/** Rep notify for bIsDead */
	UFUNCTION()
	void OnRep_IsDead();

private:
	TMap<EWeaponSlot, TObjectPtr<USkeletalMeshComponent>> WeaponMeshes;

};
