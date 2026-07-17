// Brandon Hillig 2026

#include "Controllers/ZombieAIController.h"

#include "Components/StateTreeAIComponent.h"

AZombieAIController::AZombieAIController()
{
	// Create components
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAI");
}
