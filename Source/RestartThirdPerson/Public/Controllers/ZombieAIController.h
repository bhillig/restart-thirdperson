// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

class UStateTreeAIComponent;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeAI;

public:
	AZombieAIController();
};
