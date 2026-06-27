// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CloseRangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UBTTask_CloseRangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;

	UFUNCTION()
	void OnAttackFinished(bool bInterrupted);

};
