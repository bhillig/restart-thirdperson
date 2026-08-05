// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RSZombiesGameState.generated.h"

UENUM(BlueprintType)
enum class ERSRoundPhase : uint8
{
	InProgress,
	Intermission
};

USTRUCT(BlueprintType)
struct FRSRoundState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 RoundNumber = 0;

	UPROPERTY(BlueprintReadOnly)
	ERSRoundPhase RoundPhase = ERSRoundPhase::InProgress;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnRoundStartedDelegate, int32, RoundNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnRoundCompletedDelegate, int32, RoundNumber);

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API ARSZombiesGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	ARSZombiesGameState();

	/** Register replicated variables */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Sets the round state. Called by the GameMode */
	void SetRoundState(int32 RoundNumber, ERSRoundPhase RoundPhase);

public:
	/** On Round Started Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnRoundStartedDelegate OnRoundStarted;

	/** On Round Completed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnRoundCompletedDelegate OnRoundCompleted;

protected:
	/** Current round state */
	UPROPERTY(ReplicatedUsing=OnRep_RoundState)
	FRSRoundState RoundState;

	/** Rep notify for RoundState */
	UFUNCTION()
	void OnRep_RoundState(const FRSRoundState& OldRoundState);

	/** Broadcast delegates when round state changes. Called on server and rep notify (for clients) */
	void HandleRoundStateChange(const FRSRoundState& OldRoundState);
};
