// Brandon Hillig 2026


#include "AI/RSZombieSTUtility.h"
#include "StateTreeExecutionContext.h"
#include "Character/Zombies/ZombieCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
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

	const FVector ZombieLocation = ZombiePawn->GetActorLocation();

	APawn* ClosestPlayer = nullptr;
	float ClosestDistSqr = TNumericLimits<float>::Max();

	const UWorld* World = ZombiePawn->GetWorld();
	for (FConstPlayerControllerIterator ControllerIt = World->GetPlayerControllerIterator(); ControllerIt; ++ControllerIt)
	{
		AController* PC = ControllerIt->Get();	
		if (!PC)
		{
			continue;
		}

		APawn* PlayerPawn = PC->GetPawn();
		if (!CanTargetPlayer(PlayerPawn))
		{
			continue;
		}

		const float DistSqr = FVector::DistSquared(ZombieLocation, PlayerPawn->GetActorLocation());
		if (DistSqr < ClosestDistSqr)
		{
			ClosestPlayer = PlayerPawn;
			ClosestDistSqr = DistSqr;
		}
	}

	if (ClosestPlayer)
	{
		// Set the player actor
		InstanceData.PlayerActor = ClosestPlayer;

		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}
