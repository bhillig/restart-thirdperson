// Brandon Hillig 2026


#include "Character/Zombies/ZombieCharacter.h"

#include "BrainComponent.h"
#include "ActorComponents/AttributesComponent.h"
#include "Controllers/ZombieAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AZombieCharacter::AZombieCharacter()
{
	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>("AttributesComponent");
}

void AZombieCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnTakePointDamage.AddDynamic(this, &AZombieCharacter::OnZombieTakePointDamage);
	AttributesComponent->OnDeath.AddDynamic(this, &AZombieCharacter::OnZombieDeath);
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

bool AZombieCharacter::IsDead()
{
	return bIsDead;
}

void AZombieCharacter::OnZombieTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
                                               FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (FMath::IsNearlyZero(AttributesComponent->GetHealth()))
	{
		// Let the OnDeath callback handle death
		return;
	}

	// Play fire react montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{

		FTimerDelegate Delegate;

		const EMovementMode OldMovementMode = GetCharacterMovement()->GetGroundMovementMode();

		Delegate.BindLambda([this, OldMovementMode]()
			{
				if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
				{
					CharacterMovement->SetMovementMode(OldMovementMode);
				}
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

void AZombieCharacter::OnZombieDeath(AController* EventInstigator, AActor* DamageCauser)
{
	bIsDead = true;

	// Stop character movement and collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	// Disable AI input
	if (AZombieAIController* AICon = Cast<AZombieAIController>(GetController()))
	{
		AICon->GetBrainComponent()->StopLogic(TEXT("Death"));
		AICon->StopMovement();
	}

	// Play death montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float Duration = AnimInstance->Montage_Play(DeathMontage);
		if (Duration > 0.f)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AZombieCharacter::OnDeathMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate);
		}
	}
}

void AZombieCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// Lock corpse in death pose
	GetMesh()->SetComponentTickEnabled(false);

	// Destroy zombie character after duration
	SetLifeSpan(CorpseLifeSpanAfterDeath);
}
