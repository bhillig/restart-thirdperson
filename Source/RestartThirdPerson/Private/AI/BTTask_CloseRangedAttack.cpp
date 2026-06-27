// Brandon Hillig 2026


#include "AI/BTTask_CloseRangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Zombies/ZombieCharacter.h"

EBTNodeResult::Type UBTTask_CloseRangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	check(BB);

	AAIController* AIController = OwnerComp.GetAIOwner();
	check(AIController);

	AZombieCharacter* ZombieCharacter = CastChecked<AZombieCharacter>(AIController->GetPawn());

	// Fetch TargetActor
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	// Attach to delegate
	//ZombieCharacter->OnAttackFinished.AddDynamic(this,)

	if (!ZombieCharacter->Attack(TargetActor))
	{
		// Remove delegate
		return EBTNodeResult::Failed;
	}

	// TODO: Change this to be in progress
	return EBTNodeResult::Succeeded;
}

void UBTTask_CloseRangedAttack::OnAttackFinished(bool bInterrupted)
{
	//return EBTNodeResult::Succeeded;
}
