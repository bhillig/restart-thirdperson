// Brandon Hillig 2026


#include "AI/RSZombieSTUtility.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_StateTreeEvent_Zombie_StartChasing, "StateTreeEvent.Zombie.StartChasing", "State Tree Event for when a zombie starts chasing a target");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_StateTreeEvent_Zombie_StopChasing, "StateTreeEvent.Zombie.StopChasing", "State Tree Event for when a zombie stops chasing a target");

EStateTreeRunStatus FRSStateTreeGetPlayerTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// get the instance data
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		// set the player actor
		InstanceData.PlayerActor = UGameplayStatics::GetPlayerPawn(InstanceData.Controller, 0);
	}
	return EStateTreeRunStatus::Running;
}
