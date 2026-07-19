// Brandon Hillig 2026


#include "AI/RSZombieSTUtility.h"
#include "StateTreeExecutionContext.h"
#include "Character/Zombies/ZombieCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

EStateTreeRunStatus FRSStateTreeAttackTargetTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// Get the instance data
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		// Ensure the target is valid
		if (IsValid(InstanceData.TargetActor))
		{
			// Attack the target
			InstanceData.Character->Attack(InstanceData.TargetActor);
		}
	}

	return EStateTreeRunStatus::Running;
}

////////////////////////////////////////////////////////////////////

EStateTreeRunStatus FRSStateTreeGetPlayerTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// Get the instance data
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		// Set the player actor
		InstanceData.PlayerActor = UGameplayStatics::GetPlayerPawn(InstanceData.Controller, 0);
	}
	return EStateTreeRunStatus::Running;
}
