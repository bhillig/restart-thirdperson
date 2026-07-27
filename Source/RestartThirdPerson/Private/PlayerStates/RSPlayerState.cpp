// Brandon Hillig 2026


#include "PlayerStates/RSPlayerState.h"

#include "Net/UnrealNetwork.h"

ARSPlayerState::ARSPlayerState()
{
	AvailableCredits = 0;
}

void ARSPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARSPlayerState, AvailableCredits);
}

void ARSPlayerState::AddCredits(int32 CreditsToAdd)
{
	if (!HasAuthority())
	{
		return;
	}

	AvailableCredits += CreditsToAdd;

	// Broadcast credits change for server
	OnCreditsChanged.Broadcast(AvailableCredits, CreditsToAdd);
}

bool ARSPlayerState::TrySpendCredits(int32 CreditsRequired)
{
	if (!HasAuthority())
	{
		return false;
	}

	if (AvailableCredits < CreditsRequired)
	{
		return false;
	}

	AvailableCredits -= CreditsRequired;

	// Broadcast credits change for server
	OnCreditsChanged.Broadcast(AvailableCredits, CreditsRequired);
	return true;
}

int32 ARSPlayerState::GetAvailableCredits() const
{
	return AvailableCredits;
}

void ARSPlayerState::OnRep_Credits(int32 OldCredits)
{
	// Broadcast credits change for clients
	OnCreditsChanged.Broadcast(AvailableCredits, AvailableCredits - OldCredits);
}
