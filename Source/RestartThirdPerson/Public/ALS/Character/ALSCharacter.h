// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALSCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UENUM(BlueprintType)
enum class EWeapon : uint8
{
	Unarmed = 0,
	Pistol = 1,
	Rifle = 2
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSwitched, EWeapon, Weapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGateSwitched, EGate, Gate);


UCLASS()
class RESTARTTHIRDPERSON_API AALSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AALSCharacter();

	UPROPERTY(BlueprintAssignable)
	FOnWeaponSwitched OnWeaponSwitched;

	UPROPERTY(BlueprintAssignable)
	FOnGateSwitched OnGateSwitched;

public:

	// Returns Distance From Ground, 0.f if on ground, -1.f if not above ground.
	float GetDistanceFromGround() const { return DistanceFromGround; }

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

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
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleCrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UnequipWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipPrimaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipSecondaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleSlowMotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeapon CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGate CurrentGate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	TMap<EGate, FGateSettings> GateSettingsMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement")
	float DistanceFromGround;

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

private:

	void OnMoveTriggered(const FInputActionValue& Value);

	void OnLookTriggered(const FInputActionValue& Value);

	void OnAimStarted();

	void OnAimCompleted();

	void OnCrouchToggled();

	void SwitchGate(EGate Gate);

	void OnUnequipWeaponPressed();

	void OnPrimaryWeaponEquippedPressed();

	void OnSecondaryWeaponEquippedPressed();

	void ToggleSlowMotion();

	void SwitchWeapon(EWeapon Weapon);

	void UpdateAnimInstanceForWeapon(EWeapon Weapon);

};
