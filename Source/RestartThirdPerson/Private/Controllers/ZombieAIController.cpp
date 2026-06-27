// Brandon Hillig 2026


#include "Controllers/ZombieAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AZombieAIController::AZombieAIController()
{
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	ensure(BehaviorTree);
	RunBehaviorTree(BehaviorTree);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	check(PlayerPawn);

	GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerPawn);
}
