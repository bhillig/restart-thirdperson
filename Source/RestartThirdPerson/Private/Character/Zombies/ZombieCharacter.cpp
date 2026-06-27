// Brandon Hillig 2026


#include "Character/Zombies/ZombieCharacter.h"

#include "BrainComponent.h"
#include "ActorComponents/AttributesComponent.h"
#include "Controllers/ZombieAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AZombieCharacter::AZombieCharacter()
{
	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>("AttributesComponent");
}

void AZombieCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributesComponent->OnHealthChanged.AddDynamic(this, &AZombieCharacter::OnHealthChanged);
	AttributesComponent->OnDeath.AddDynamic(this, &AZombieCharacter::OnDeath);
}

bool AZombieCharacter::Attack(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return false;
	}

	// Face the actor we are attacking
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());
	SetActorRotation(LookAtRotation);

	// Play attack montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(AttackMontage);
	}

	return true;
}

void AZombieCharacter::OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser)
{
	if (FMath::IsNearlyZero(NewHealth))
	{
		// Let the OnDeath callback handle death
		return;
	}


	if (Delta < 0.f)
	{
		// We were dealt damage so react

		// Play fire react montage
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{

			FTimerDelegate Delegate;

			const EMovementMode OldMovementMode = GetCharacterMovement()->GetGroundMovementMode();

			Delegate.BindLambda([this, OldMovementMode]()
				{
					GetCharacterMovement()->SetMovementMode(OldMovementMode);
				});

			if (AZombieAIController* AICon = Cast<AZombieAIController>(GetController()))
			{
				AICon->StopMovement();
			}
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->SetMovementMode(MOVE_None);
			const float MontageLength = AnimInstance->Montage_Play(FireReactMontage);

			GetWorldTimerManager().SetTimer(FireReactTimerHandle, Delegate, MontageLength, false);
		}
	}
}

void AZombieCharacter::OnDeath(AController* EventInstigator, AActor* DamageCauser)
{
	// Disable AI input
	if (AZombieAIController* AICon = Cast<AZombieAIController>(GetController()))
	{
		AICon->GetBrainComponent()->StopLogic(TEXT("Death"));
		AICon->StopMovement();
	}

	// Play death montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{

		FTimerDelegate Delegate;

		Delegate.BindLambda([this]()
			{
				Destroy();
			});

		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		// TODO: Remove hack
		const float MontageLength = AnimInstance->Montage_Play(DeathMontage) - 0.3f;

		GetWorldTimerManager().SetTimer(DeathTimerHandle, Delegate, MontageLength, false);
	}
	else
	{
		Destroy();
	}
}
