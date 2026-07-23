// Brandon Hillig 2026


#include "GameMode/RSZombiesGameMode.h"
#include "Character/Zombies/ZombieCharacter.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

ARSZombiesGameMode::ARSZombiesGameMode()
{
	CurrentRoundNumber = 0;
}

void ARSZombiesGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Grab all the spawn positions
	UGameplayStatics::GetAllActorsWithTag(this, FName("ZombieSpawnPoint"), SpawnPoints);

	// Start round 1
	AdvanceRound();
}

void ARSZombiesGameMode::AdvanceRound()
{
	// Increase round number
	CurrentRoundNumber++;

	// Calculate the zombies needed this round
	ZombiesLeftToSpawnThisRound = GetZombieCountForRound(CurrentRoundNumber, 1);

	// Begin spawner
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ARSZombiesGameMode::TrySpawnZombie);
	GetWorldTimerManager().SetTimer(TimerHandle_ZombieSpawn, Delegate, SpawnInterval, true, 3.f);

	// Broadcast new round started
	OnNewRoundStarted.Broadcast(CurrentRoundNumber);
}

void ARSZombiesGameMode::CompleteRound()
{
	// Clear current timer
	TimerHandle_ZombieSpawn.Invalidate();

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ARSZombiesGameMode::AdvanceRound);

	// Wait duration before starting the next round
	GetWorldTimerManager().SetTimer(TimerHandle_RoundBreak, Delegate, RoundBreakDuration, false);

	// Advance to the next round
	AdvanceRound();
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

void ARSZombiesGameMode::TrySpawnZombie()
{
	if (ZombiesLeftToSpawnThisRound > 0 && ZombiesAlive < GetConcurrentZombieCap(1))
	{
		ensureMsgf(!SpawnPoints.IsEmpty(), TEXT("Spawn points is empty"));

		// Pick a random spawn position
		const int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		const FVector SpawnPosition = SpawnPoints[RandomIndex]->GetActorLocation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Spawn a zombie
		if (AZombieCharacter* ZombieCharacter = GetWorld()->SpawnActor<AZombieCharacter>(ZombieClass.Get(), SpawnPosition, FRotator::ZeroRotator, SpawnParams))
		{
			ZombieCharacter->OnPawnDeath.AddDynamic(this, &ARSZombiesGameMode::OnZombieDeath);
			ZombiesLeftToSpawnThisRound--;
			rs::LogOnce("Spawned zombie!");
		}
	}
}

void ARSZombiesGameMode::OnZombieDeath()
{
	ZombiesAlive--;
	rs::LogInt("ZombiesLeft", ZombiesLeftToSpawnThisRound, FColor::White, 3.0f);

	if (ZombiesLeftToSpawnThisRound == 0)
	{
		// Round complete
		CompleteRound();
	}
}
