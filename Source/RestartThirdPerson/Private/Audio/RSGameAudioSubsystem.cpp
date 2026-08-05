// Brandon Hillig 2026


#include "Audio/RSGameAudioSubsystem.h"

#include "GameStates/RSZombiesGameState.h"
#include "Kismet/GameplayStatics.h"

bool URSGameAudioSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Type::Game || WorldType == EWorldType::Type::PIE;
}

void URSGameAudioSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Load game audio catalog
	const URSGameAudioSubsystemSettings* GameAudioSubsystemSettings = GetDefault<URSGameAudioSubsystemSettings>();
	GameAudioCatalog = GameAudioSubsystemSettings->GameAudioCatalog.LoadSynchronous();

	// Try to get the game state
	if (AGameStateBase* GameState = InWorld.GetGameState())
	{
		// Must be on the server
		BindGameState(GameState);
	}
	else
	{
		// Listen for replication
		InWorld.GameStateSetEvent.AddUObject(this, &URSGameAudioSubsystem::BindGameState);
	}
}

void URSGameAudioSubsystem::BindGameState(AGameStateBase* GameState)
{
	ARSZombiesGameState* ZombiesGameState = Cast<ARSZombiesGameState>(GameState);
	if (!ZombiesGameState || bBound)
	{
		return;
	}

	ZombiesGameState->OnRoundStarted.AddDynamic(this, &URSGameAudioSubsystem::HandleRoundStarted);
	ZombiesGameState->OnRoundCompleted.AddDynamic(this, &URSGameAudioSubsystem::HandleRoundCompleted);
	bBound = true;
}

void URSGameAudioSubsystem::HandleRoundStarted(int32 RoundNumber)
{
	if (GameAudioCatalog && GameAudioCatalog->RoundStarted)
	{
		UGameplayStatics::PlaySound2D(this, GameAudioCatalog->RoundStarted, 0.3f);
	}
}

void URSGameAudioSubsystem::HandleRoundCompleted(int32 RoundNumber)
{
	if (GameAudioCatalog && GameAudioCatalog->RoundCompleted)
	{
		UGameplayStatics::PlaySound2D(this, GameAudioCatalog->RoundCompleted, 0.3f);
	}
}
