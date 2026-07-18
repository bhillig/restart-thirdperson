// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "NativeGameplayTags.h"
#include "RSZombieSTUtility.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_StateTreeEvent_Zombie_StartChasing);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_StateTreeEvent_Zombie_StopChasing);

class AAIController;

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
USTRUCT()
struct FRSStateTreeGetPlayerTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	/** Get the instance data for this task */
	using FInstanceDataType = FRSStateTreeGetPlayerInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/** Runs when the owning state is entered */
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};