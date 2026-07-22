// Brandon Hillig 2026


#include "GameMode/RSZombiesGameMode.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void ARSZombiesGameMode::BeginPlay()
{
	Super::BeginPlay();

	const int32 ZombiesToSpawn = GetZombieCountForRound(CurrentRoundNumber, 1);
	rs::LogInt("ZombiesToSpawn", ZombiesToSpawn, FColor::Orange, 10.f);
}

int32 ARSZombiesGameMode::GetZombieCountForRound(int32 RoundNumber, int32 PlayerCount)
{
	ensureMsgf(RoundNumber > 0, TEXT("Round number must be at least 1"));
	ensureMsgf(PlayerCount > 0, TEXT("Player count must be at least 1"));

	const bool bIsSolo = PlayerCount == 1;

	const float ScaleFactor = bIsSolo ? 0.5f : 0.5f + (PlayerCount / 2.f);

	// Before Round 10
	if (RoundNumber < 10)
	{
		float PostScaleFactor = 0.f;
		switch (RoundNumber)
		{
		case 1:
			PostScaleFactor = 0.25f;
			break;
		case 2:
			PostScaleFactor = 0.3f;
			break;
		case 3:
			PostScaleFactor = 0.7f;
			break;
		case 4:
			PostScaleFactor = 0.9f;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			PostScaleFactor = 1.f;
			break;
		default:
			break;
		}

		return FMath::Floor((24 + (ScaleFactor * 6 * FMath::Max(1, RoundNumber / 5.f)) * PostScaleFactor));
	}

	// Round 10 And Above
	return FMath::Floor(24 + (ScaleFactor * 6 * (RoundNumber/5.f) * RoundNumber * 0.15f));
}

int32 ARSZombiesGameMode::GetConcurrentZombieCap(int32 PlayerCount)
{
	ensureMsgf(PlayerCount > 0, TEXT("Player count must be at least 1"));
	return 24 + 6 * (PlayerCount - 1);
}