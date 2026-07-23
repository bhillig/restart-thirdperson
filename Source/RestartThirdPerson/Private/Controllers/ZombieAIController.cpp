// Brandon Hillig 2026

#include "Controllers/ZombieAIController.h"

#include "Character/Zombies/ZombieCharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "Navigation/PathFollowingComponent.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_StateTreeEvent_Zombie_StartStunned, "StateTreeEvent.Zombie.StartStunned", "StateTree event for when a zombie is stunned");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_StateTreeEvent_Zombie_EndStunned, "StateTreeEvent.Zombie.EndStunned", "StateTree event for when a zombie is no longer stunned");

AZombieAIController::AZombieAIController()
{
	// Create components
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAI");
	StateTreeAI->SetStartLogicAutomatically(false);
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(InPawn))
	{
		// Subscribe to the zombie's delegates
		ZombieCharacter->OnPawnDeath.AddDynamic(this, &AZombieAIController::OnPawnDeath);
		ZombieCharacter->OnPawnStunned.AddDynamic(this, &AZombieAIController::OnPawnStunned);
		ZombieCharacter->OnPawnNoLongerStunned.AddDynamic(this, &AZombieAIController::OnPawnNoLongerStunned);
	}

	// Start AI Logic
	StateTreeAI->StartLogic();
}

void AZombieAIController::OnPawnDeath()
{
	// Stop path following
	if (UPathFollowingComponent* PathFollowComp = GetPathFollowingComponent())
	{
		PathFollowComp->AbortMove(*this, FPathFollowingResultFlags::UserAbort);
	}

	// Stop AI Logic
	StateTreeAI->StopLogic(FString("Death"));

	// Unpossess the pawn
	UnPossess();

	// Destroy this controller
	Destroy();
}

void AZombieAIController::OnPawnStunned()
{
	// Send event to the state tree
	StateTreeAI->SendStateTreeEvent(TAG_StateTreeEvent_Zombie_StartStunned);
}

void AZombieAIController::OnPawnNoLongerStunned()
{
	// Send event to the state tree
	StateTreeAI->SendStateTreeEvent(TAG_StateTreeEvent_Zombie_EndStunned);
}
