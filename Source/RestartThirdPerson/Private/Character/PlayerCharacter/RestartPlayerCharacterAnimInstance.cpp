// Brandon Hillig 2026


#include "Character/PlayerCharacter/RestartPlayerCharacterAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

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
		bIsFalling = CharacterMovementComponent->IsFalling();
	}
}
