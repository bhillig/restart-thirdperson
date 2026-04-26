// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ALS/Character/ALSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "ALSCharacterAnimInstance.generated.h"


UENUM(BlueprintType)
enum class ELocomotionDirection : uint8
{
	Forward = 0,
	Backward = 1,
	Left = 2,
	Right = 3
};

UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	Hold = 0,
	BlendOut = 1,
	Accumulate = 2
};

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UALSCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	void NativeBeginPlay() override;

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;

	void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:

	// Cache References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	// Character Movement Values
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FVector LastFrameLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float DeltaDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FVector Acceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FVector GroundAcceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float VelocityDotAcceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float VelocityDirectionAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float VelocityDirectionAngleWithOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float AccelerationDirectionAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	ELocomotionDirection LastFrameVelocityLocomotionDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	ELocomotionDirection VelocityLocomotionDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	bool bChangedLocomotionDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	ELocomotionDirection AccelerationLocomotionDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	EGate LastFrameGate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	EGate CurrentGate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	bool bChangedGate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float LastFrameActorYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float ActorYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float DeltaActorYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float RootYawOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement")
	ERootYawOffsetMode RootYawOffsetMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float LeanAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FVector PredictedGroundStopLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float DistanceUntilStop;

	EGate IncomingGate;

	FFloatSpringState RootYawOffsetToZeroSpringState;

protected:

	UFUNCTION()
	void OnGateSwitched(EGate Gate);

	ELocomotionDirection CalculateLocomotionDirection(float Angle, float DeadZone, ELocomotionDirection CurrentDirection);

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void UpdateRootYawOffset(float DeltaSeconds);

};
