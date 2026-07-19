// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "RSZombieSTUtility.generated.h"

class AZombieCharacter;

class AAIController;

/**
 *  Instance data struct for the Attack Target StateTree task
 */
USTRUCT()
struct FRSStateTreeAttackTargetInstanceData
{
	GENERATED_BODY()

	/** The context zombie character */
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AZombieCharacter> Character;

	/** The actor to attack */
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<AActor> TargetActor;
};

/**
 *  StateTree task that attacks a target actor
 */
USTRUCT(meta=(DisplayName = "Attack Target", Category = "Zombie"))
struct FRSStateTreeAttackTargetTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Get the instance data for this task */
	using FInstanceDataType = FRSStateTreeAttackTargetInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

////////////////////////////////////////////////////////////////////

/**
 *  Instance data struct for the Get Player StateTree task
 */
USTRUCT()
struct FRSStateTreeGetPlayerInstanceData
{
	GENERATED_BODY()

	/** Zombie AI Controller */
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> Controller;

	/** Player actor to retrieve */
	UPROPERTY(EditAnywhere, Category = Output)
	TObjectPtr<AActor> PlayerActor;
};

/**
 *  StateTree task that gets the player actor
 */
USTRUCT(meta=(DisplayName = "Get Player", Category = "Zombie"))
struct FRSStateTreeGetPlayerTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Get the instance data for this task */
	using FInstanceDataType = FRSStateTreeGetPlayerInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};