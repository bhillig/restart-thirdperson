// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RSPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRSOnCreditsChangedDelegate, int32, NewCreditsAvailable, int32, Delta);

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API ARSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	/** Constructor */
	ARSPlayerState();

	/** Registers replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Adds credits to this player's available credits */
	void AddCredits(int32 CreditsToAdd);

	/** Attempts to spend credits required from a player's available credits.
	 *  Returns true if the player has enough and was successful, false otherwise. (E.g used by vendors/doors)
	 */
	bool TrySpendCredits(int32 CreditsRequired);

	/** Returns this player's available credits */
	int32 GetAvailableCredits() const;

public:
	/** On Credits Changed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnCreditsChangedDelegate OnCreditsChanged;

protected:
	/** Player's available credits */
	UPROPERTY(ReplicatedUsing=OnRep_Credits)
	int32 AvailableCredits;

protected:
	/** Rep notify for AvailableCredits */
	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);
};
