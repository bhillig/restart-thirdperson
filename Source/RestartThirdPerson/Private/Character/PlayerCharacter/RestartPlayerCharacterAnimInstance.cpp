// Brandon Hillig 2026


#include "Character/PlayerCharacter/RestartPlayerCharacterAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"
#include "Kismet/KismetMathLibrary.h"

static TAutoConsoleVariable CVarDebugCharacterAnimInstance(TEXT("Debug.PlayerAnimInstance"), false, TEXT("Toggle player anim instance debug display"));

void URestartPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Get Character Movement Component
	if (ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
	{
		CharacterMovementComponent = Character->GetCharacterMovement();
	}
}

void URestartPlayerCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovementComponent)
	{
		// Get velocity
		Velocity = CharacterMovementComponent->Velocity;

		// Get ground speed
		const FVector GroundVelocity = FVector(CharacterMovementComponent->Velocity.X, CharacterMovementComponent->Velocity.Y, 0);
		GroundSpeed = FVector::Dist(FVector::Zero(), GroundVelocity);

		// Determine if the player character is falling
		bIsInAir = CharacterMovementComponent->IsFalling();

		// Determine if the player character is accelerating
		bIsAccelerating = CharacterMovementComponent->GetCurrentAcceleration().Length() > 0;

		// Calculate Roll, Pitch, and Yaw (difference from where the controller is facing and the actor)
		const FRotator BaseAimRotation = TryGetPawnOwner()->GetBaseAimRotation();
		const FRotator ActorRotation = TryGetPawnOwner()->GetActorRotation();
		const FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(BaseAimRotation, ActorRotation);
		Roll = DeltaRotator.Roll;
		Pitch = DeltaRotator.Pitch;
		Yaw = DeltaRotator.Yaw;

		// Get whether the current animation is requesting to override the full body
		bIsFullbody = GetCurveValue(FName("FullBody")) > 0.f;

		if (CVarDebugCharacterAnimInstance.GetValueOnGameThread())
		{
			rs::LogTick(FString::Printf(TEXT("Ground Speed: %f"), GroundSpeed), 4, FColor::Blue);
			rs::LogTick(FString::Printf(TEXT("Is In Air: %s"), bIsInAir ? TEXT("True") : TEXT("False")), 0, FColor::Purple);
			rs::LogTick(FString::Printf(TEXT("Is Accelerating: %s"), bIsAccelerating ? TEXT("True") : TEXT("False")), 5, FColor::Yellow);
			rs::LogTick(FString::Printf(TEXT("Base aim rotation: %s"), *TryGetPawnOwner()->GetBaseAimRotation().ToString()), 1, FColor::Green);
			rs::LogTick(FString::Printf(TEXT("Actor rotation: %s"), *TryGetPawnOwner()->GetActorRotation().ToString()), 2, FColor::Orange);
			rs::LogTick(FString::Printf(TEXT("Roll: %.2f, Pitch: %.2f, Yaw: %.2f"), Roll, Pitch, Yaw), 3, FColor::Cyan);
			rs::LogTick(FString::Printf(TEXT("Is FullBody: %s"), bIsFullbody ? TEXT("True") : TEXT("False")), 8, FColor::Emerald);
		}
	}
}
