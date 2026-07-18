// Brandon Hillig 2026


#include "Character/Zombies/ZombieCharacter.h"

#include "ActorComponents/AttributesComponent.h"
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
	// Ensure the target is valid
	if (!IsValid(TargetActor))
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

void AZombieCharacter::OnZombieTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (FMath::IsNearlyZero(AttributesComponent->GetHealth()))
	{
		// Let the OnDeath callback handle death
		return;
	}

	// Stop moving
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		CharacterMovementComp->StopMovementImmediately();
		CharacterMovementComp->SetMovementMode(MOVE_None);
	}

	// Play fire react montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float Duration = AnimInstance->Montage_Play(FireReactMontage);
		if (Duration > 0.f)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AZombieCharacter::OnFireReactMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate);
		}
	}
}

void AZombieCharacter::OnFireReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// Something else took over the montage slot (another react, or death) - whatever
	// interrupted us owns the movement mode now, so leave it alone
	if (bInterrupted)
	{
		return;
	}

	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		CharacterMovementComp->SetMovementMode(MOVE_Walking);
	}
}

void AZombieCharacter::OnZombieDeath(AController* EventInstigator, AActor* DamageCauser)
{
	// Disable collision
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (USkeletalMeshComponent* ZombieMesh = GetMesh())
	{
		ZombieMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Stop character movement
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		CharacterMovementComp->StopMovementImmediately();
		CharacterMovementComp->SetMovementMode(MOVE_None);
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

	// Destroy zombie character after duration
	SetLifeSpan(CorpseLifeSpanAfterDeath);

	// Emit a signal we died
	OnPawnDeath.Broadcast();
}

void AZombieCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// Lock corpse in death pose
	if (USkeletalMeshComponent* ZombieMeshComp = GetMesh())
	{
		ZombieMeshComp->SetComponentTickEnabled(false);
	}
}
