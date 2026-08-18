// Brandon Hillig 2026


#include "AI/RSZombieSTUtility.h"
#include "StateTreeExecutionContext.h"
#include "Character/Zombies/ZombieCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "AI/RSZombieDirectorSubsystem.h"
#include "ALS/Character/ALSCharacter.h"

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

/** Returns whether the pawn passed in is a valid player for the zombie to target */
bool CanTargetPlayer(APawn* PlayerPawn)
{
	AALSCharacter* PlayerCharacter = Cast<AALSCharacter>(PlayerPawn);
	if (!PlayerCharacter)
	{
		return false;
	}

	return !PlayerCharacter->IsDead();
}

EStateTreeRunStatus FRSStateTreeGetPlayerTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return EStateTreeRunStatus::Running;
	}

	// Get the instance data
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	APawn* ZombiePawn = InstanceData.Controller ? InstanceData.Controller->GetPawn() : nullptr;
	if (!ZombiePawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	URSZombieDirectorSubsystem* ZombieDirectorSubsystem = ZombiePawn->GetWorld()->GetSubsystem<URSZombieDirectorSubsystem>();
	ensure(ZombieDirectorSubsystem);

	InstanceData.PlayerActor = ZombieDirectorSubsystem->GetPlayerTarget(ZombiePawn);

	return EStateTreeRunStatus::Running;
}
