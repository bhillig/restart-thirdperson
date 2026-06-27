// Brandon Hillig 2026


#include "AI/BTService_CheckRangeTo.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void UBTService_CheckRangeTo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	check(BB);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	APawn* OwnerPawn = OwnerController->GetPawn();
	check(OwnerPawn);

	if (AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)))
	{
		const float DistanceTo = FVector::Dist(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation());
		const bool bWithinRange = DistanceTo < AttackDistance && OwnerController->LineOfSightTo(TargetActor);
		rs::LogFloat("DistanceTo", DistanceTo, FColor::Cyan, 0.5f);
		BB->SetValueAsBool(WithinRangeKey.SelectedKeyName, bWithinRange && OwnerController->LineOfSightTo(TargetActor));
	}
}
