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

	/** Runs the behavior StateTree for this Zombie */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeAI;

public:
	/** Constructor */
	AZombieAIController();

protected:
	/** Pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

	/** Called when the game begins */
	virtual void BeginPlay() override;

protected:
	/** Called when the possessed pawn dies */
	UFUNCTION()
	void OnPawnDeath();
};
