// Brandon Hillig 2026


#include "Character/Zombies/ZombieCharacter.h"

#include "ActorComponents/AttributesComponent.h"
#include "ActorComponents/RSZombieVoiceComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

AZombieCharacter::AZombieCharacter()
{
	// Setup replication
	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	// Set default values
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Create components
	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>("AttributesComponent");

	ZombieVoiceComponent = CreateDefaultSubobject<URSZombieVoiceComponent>("ZombieVoiceComponent");
	ZombieVoiceComponent->SetupAttachment(GetMesh(), FName("head"));

	// Set capsule and mesh to ignore the Ground Trace Channel. Used for IK Traces and should ignore pawns
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Ground, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Ground, ECR_Ignore);
}

void AZombieCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsDead);
}

void AZombieCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnTakePointDamage.AddDynamic(this, &AZombieCharacter::OnZombieTakePointDamage);
	AttributesComponent->OnDeath.AddDynamic(this, &AZombieCharacter::OnZombieDeath);

	ZombieVoiceComponent->Enable();
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

void AZombieCharacter::Multicast_PlayAnimMontage_Implementation(UAnimMontage* Montage)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(Montage);
	}
}

void AZombieCharacter::OnRep_IsDead()
{
	HandleZombieDeath();
}

void AZombieCharacter::OnZombieTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return;
	}

	// Must be on the server
	// Cache whether this damage hit the head. Used in OnZombieDeath to broadcast headshot kill.
	bLastShotWasAHeadshot = BoneName == HeadBoneName;

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

	GetWorldTimerManager().SetTimer(TimerHandle_FireReactMontageElapsed, this, &AZombieCharacter::OnFireReactMontageEnded, HitReactMontageDuration);
	Multicast_PlayAnimMontage(FireReactMontage);

	// Broadcast hit
	OnPawnHit.Broadcast(InstigatedBy);
}

void AZombieCharacter::OnFireReactMontageEnded()
{
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
	if (!HasAuthority())
	{
		return;
	}

	// Must be on the server
	bIsDead = true;

	// Destroy zombie character after duration
	SetLifeSpan(CorpseLifeSpanAfterDeath);

	// Emit a signal we died for AIController (Server-Only)
	OnPawnDeath.Broadcast(EventInstigator, bLastShotWasAHeadshot);

	HandleZombieDeath();
}

void AZombieCharacter::HandleZombieDeath()
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

	// Stop audio
	if (ZombieVoiceComponent)
	{
		ZombieVoiceComponent->Disable();
	}

	// Play Death Montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(DeathMontage);
		GetWorldTimerManager().SetTimer(TimerHandle_DeathMontageElapsed, this, &AZombieCharacter::OnDeathMontageEnded, DeathMontageDuration);
	}
}

void AZombieCharacter::OnDeathMontageEnded()
{
	// Lock corpse in death pose
	if (USkeletalMeshComponent* ZombieMeshComp = GetMesh())
	{
		ZombieMeshComp->SetComponentTickEnabled(false);
	}
}
