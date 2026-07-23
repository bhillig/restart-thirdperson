// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NativeGameplayTags.h"
#include "ZombieAIController.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_StateTreeEvent_Zombie_StartStunned);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_StateTreeEvent_Zombie_EndStunned);

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

protected:
	/** Called when the possessed pawn dies */
	UFUNCTION()
	void OnPawnDeath();

	/** Called when the possessed pawn is stunned */
	UFUNCTION()
	void OnPawnStunned();


	/** Called when the possessed pawn is no longer stunned */
	UFUNCTION()
	void OnPawnNoLongerStunned();
};
