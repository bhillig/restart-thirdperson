// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RestartPlayerCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URestartPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Roll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsFullbody;

};
