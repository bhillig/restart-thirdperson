// Brandon Hillig 2026


#include "Utility/RSGameplayLibrary.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

int32 URSGameplayLibrary::GetPlayerCount(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	if (!GameState)
	{
		return 0;
	}

	int32 PlayerCount = 0;
	for (const APlayerState* PlayerState : GameState->PlayerArray)
	{
		// Exclude spectators and bots
		if (PlayerState && !PlayerState->IsOnlyASpectator() && !PlayerState->IsABot())
		{
			PlayerCount++;
		}
	}
	return PlayerCount;
}
