// Brandon Hillig 2026


#include "Controllers/RSPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ALS/Character/ALSCharacter.h"
#include "Debug/RSCheatManager.h"
#include "GameStates/RSZombiesGameState.h"
#include "PlayerStates/RSPlayerState.h"

ARSPlayerController::ARSPlayerController()
{
	CheatClass = URSCheatManager::StaticClass();
}

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set input mode to game only
	SetInputMode(FInputModeGameOnly());

	if (IsLocalPlayerController())
	{
		// Add mapping context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

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

void ARSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_CheckScoreboard, ETriggerEvent::Started, this, &ThisClass::OnCheckScoreboardPressed);
		EIC->BindAction(IA_CheckScoreboard, ETriggerEvent::Completed, this, &ThisClass::OnCheckScoreboardReleased);
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
