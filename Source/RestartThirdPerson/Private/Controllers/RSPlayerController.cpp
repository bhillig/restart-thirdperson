// Brandon Hillig 2026


#include "Controllers/RSPlayerController.h"

#include "ALS/Character/ALSCharacter.h"
#include "GameStates/RSZombiesGameState.h"
#include "PlayerStates/RSPlayerState.h"

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		CreateUserWidgets();

		// Try to get the game state
		if (ARSZombiesGameState* ZombiesGameState = Cast<ARSZombiesGameState>(GetWorld()->GetGameState()))
		{
			// Must be on the server
			BindToGameState(ZombiesGameState);
		}
		else
		{
			// Listen for replication
			GetWorld()->GameStateSetEvent.AddLambda([this](AGameStateBase* GameState)
				{
					if (ARSZombiesGameState* ZombiesGameState = Cast<ARSZombiesGameState>(GameState))
					{
						BindToGameState(ZombiesGameState);
					}
				});
		}

		// Try to get the player state
		if (ARSPlayerState* PS = GetPlayerState<ARSPlayerState>())
		{
			// Must be on server
			BindToPlayerState(PS);
		}
	}
}

void ARSPlayerController::AcknowledgePossession(APawn* OwningPawn)
{
	Super::AcknowledgePossession(OwningPawn);

	if (!IsLocalPlayerController())
	{
		return;
	}

	if (AALSCharacter* PlayerCharacter = Cast<AALSCharacter>(OwningPawn))
	{
		BindToPlayerCharacter(PlayerCharacter);
	}
}

void ARSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ARSPlayerState* PS = GetPlayerState<ARSPlayerState>())
	{
		BindToPlayerState(PS);
	}
}
