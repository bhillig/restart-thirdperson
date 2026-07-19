// Brandon Hillig 2026


#include "Character/Zombies/ZombieCharacter.h"

#include "ActorComponents/AttributesComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

AZombieCharacter::AZombieCharacter()
{
	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>("AttributesComponent");

	// Set capsule and mesh to ignore the Ground Trace Channel. Used for IK Traces and should ignore pawns
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Ground, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Ground, ECR_Ignore);
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

	// Cache the actor the zombie is attacking
	VictimActor = TargetActor;

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

bool AZombieCharacter::PerformHitCheck()
{
	FCollisionShape Shape;
	Shape.SetSphere(30.f);

	FCollisionQueryParams QueryParams(TEXT("ZombieAttack"));
	QueryParams.AddIgnoredActor(this);

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	// Perform hit check on the actor we are attacking
	FHitResult OutHit;
	bool bHitVictim = false;
	if (GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Pawn, Shape, QueryParams))
	{
		// If this is the actor we are attacking
		bHitVictim = OutHit.GetActor() == VictimActor;
		if (bHitVictim)
		{
			// Apply damage
			UGameplayStatics::ApplyPointDamage(VictimActor, DamagePerHit, OutHit.ImpactNormal, OutHit, GetController(), this, UDamageType::StaticClass());
		}
	}

	// Clear victim from cache
	VictimActor = nullptr;

	return bHitVictim;
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

	// Broadcast stunned
	OnPawnStunned.Broadcast();
}

void AZombieCharacter::OnFireReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// Something else took over the montage slot (another react, or death) - whatever
	// interrupted us owns the movement mode now, so leave it alone
	if (bInterrupted)
	{
		return;
	}

	// Resume movement
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		CharacterMovementComp->SetMovementMode(MOVE_Walking);
	}

	// Broadcast no longer stunned
	OnPawnNoLongerStunned.Broadcast();
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
