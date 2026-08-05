// Brandon Hillig 2026


#include "GameStates/RSZombiesGameState.h"

#include "Net/UnrealNetwork.h"

ARSZombiesGameState::ARSZombiesGameState()
{
	bNetLoadOnClient = true;
	bReplicates = true;
}

void ARSZombiesGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RoundState);
}

void ARSZombiesGameState::SetRoundState(int32 RoundNumber, ERSRoundPhase RoundPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	// Only called on server
	const FRSRoundState OldRoundState = RoundState;
	RoundState.RoundNumber = RoundNumber;
	RoundState.RoundPhase = RoundPhase;

	HandleRoundStateChange(OldRoundState);
}

void ARSZombiesGameState::OnRep_RoundState(const FRSRoundState& OldRoundState)
{
	HandleRoundStateChange(OldRoundState);
}

void ARSZombiesGameState::HandleRoundStateChange(const FRSRoundState& OldRoundState)
{
	// If we are starting a new round
	if (OldRoundState.RoundNumber < RoundState.RoundNumber)
	{
		OnRoundStarted.Broadcast(RoundState.RoundNumber);
	}
	// If we completed a round
	else if (OldRoundState.RoundPhase == ERSRoundPhase::InProgress && RoundState.RoundPhase == ERSRoundPhase::Intermission)
	{
		OnRoundCompleted.Broadcast(RoundState.RoundNumber);
	}
}
