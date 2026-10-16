// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RSPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRSOnCreditsChangedDelegate, int32, NewCreditsAvailable, int32, Delta);

USTRUCT(BlueprintType)
struct FRSPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 Kills = 0;
	UPROPERTY(BlueprintReadOnly) int32 Headshots = 0;
	UPROPERTY(BlueprintReadOnly) int32 Downs = 0;
	UPROPERTY(BlueprintReadOnly) int32 Revives = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnStatsChangedDelegate, const FRSPlayerStats&, NewStats);

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API ARSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Credits

	/** Adds credits to this player's available credits */
	void AddCredits(int32 CreditsToAdd);

	/** Attempts to spend credits required from a player's available credits.
	 *  Returns true if the player has enough and was successful, false otherwise. (E.g used by vendors/doors)
	 */
	bool TrySpendCredits(int32 CreditsRequired);

	/** Returns this player's available credits */
	int32 GetAvailableCredits() const;

	// Stats

	/** Adds a kill to this player's stats */
	void AddKill(bool bWasHeadshot);

public:
	/** On Credits Changed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnCreditsChangedDelegate OnCreditsChanged;

	/** On Stats Changed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnStatsChangedDelegate OnStatsChanged;

protected:
	/** Player's available credits */
	UPROPERTY(ReplicatedUsing=OnRep_Credits)
	int32 AvailableCredits = 0;

	UPROPERTY(ReplicatedUsing=OnRep_Stats)
	FRSPlayerStats Stats;

protected:
	/** Rep notify for AvailableCredits */
	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

	/** Rep notify for Stats */
	UFUNCTION()
	void OnRep_Stats(const FRSPlayerStats& OldStats);

public:
	/** Constructor */
	ARSPlayerState();

	/** Registers replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
