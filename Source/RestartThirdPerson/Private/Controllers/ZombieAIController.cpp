// Brandon Hillig 2026

#include "Controllers/ZombieAIController.h"

#include "Character/Zombies/ZombieCharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "Navigation/PathFollowingComponent.h"

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
		// Subscribe to the zombie's on death delegate
		ZombieCharacter->OnPawnDeath.AddDynamic(this, &AZombieAIController::OnPawnDeath);
	}
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	// Start AI Logic
	// TODO: Move to OnPossess once the ZombieSpawner is implemented. OnPossess is called before BeginPlay for level placed pawns so the StateTree is started on an inactive world.
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
