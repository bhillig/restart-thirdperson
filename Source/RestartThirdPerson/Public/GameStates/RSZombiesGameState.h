// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RSZombiesGameState.generated.h"

class ARSPlayerState;

UENUM(BlueprintType)
enum class ERSRoundPhase : uint8
{
	InProgress,
	Intermission,
	GameOver
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnGameOverDelegate, int32, RoundNumber);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnPlayerStateChanged, ARSPlayerState*, PlayerState);

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

	/** On Game Over Delegate. Broadcast when all players have died */
	UPROPERTY(BlueprintAssignable)
	FRSOnGameOverDelegate OnGameOver;

	/** Player State Added Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnPlayerStateChanged OnPlayerStateAdded;

	/** Player State Removed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnPlayerStateChanged OnPlayerStateRemoved;

protected:
	/** Current round state */
	UPROPERTY(ReplicatedUsing=OnRep_RoundState)
	FRSRoundState RoundState;

	/** Rep notify for RoundState */
	UFUNCTION()
	void OnRep_RoundState(const FRSRoundState& OldRoundState);

	/** Broadcast delegates when round state changes. Called on server and rep notify (for clients) */
	void HandleRoundStateChange(const FRSRoundState& OldRoundState);

public:
	// AGameStateBase Implementation
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	// AGameStateBase Implementation
};
