// Brandon Hillig 2026


#include "AI/RSZombieDirectorSubsystem.h"

#include "Character/Zombies/ZombieCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

bool URSZombieDirectorSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Type::Game || WorldType == EWorldType::Type::PIE;
}

void URSZombieDirectorSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Grab all the spawn positions
	UGameplayStatics::GetAllActorsWithTag(this, FName("ZombieSpawnPoint"), SpawnPoints);
}

AZombieCharacter* URSZombieDirectorSubsystem::SpawnZombie(TSubclassOf<AZombieCharacter> ZombieClass)
{
	// Calculate what player should be targeted
	APlayerState* Target = DetermineTarget();
	if (!Target)
	{
		return nullptr;
	}

	// Pick a random spawn position
	const int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	const FTransform SpawnTransform = SpawnPoints[RandomIndex]->GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the zombie
	AZombieCharacter* ZombieCharacter = GetWorld()->SpawnActorDeferred<AZombieCharacter>(ZombieClass.Get(), SpawnTransform);

	// Assign its target
	AssignTarget(ZombieCharacter, Target);

	ZombieCharacter->FinishSpawning(SpawnTransform);
	return ZombieCharacter;
}

APawn* URSZombieDirectorSubsystem::GetPlayerTarget(APawn* Zombie)
{
	for (const auto& [PlayerState, Zombies] : ZombiesByPlayer)
	{
		if (Zombies.Contains(Zombie))
		{
			return PlayerState->GetPawn();
		}
	}
	return nullptr;
}

APlayerState* URSZombieDirectorSubsystem::DetermineTarget() const
{
	TArray<APlayerState*> Candidates;
	TArray<float> Weights;
	float TotalWeight = 0.f;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}

		APawn* PlayerPawn = PC->GetPawn();
		if (!IsValid(PlayerPawn))
		{
			continue;
		}

		APlayerState* PS = PlayerPawn->GetPlayerState();
		if (!PS)
		{
			continue;
		}

		const int32 AssignedCount = GetAssignedCountForPlayer(PS);

		const float Weight = 1.f / (1.f + AssignedCount);

		Candidates.Add(PS);
		Weights.Add(Weight);
		TotalWeight += Weight;
	}

	if (Candidates.Num() == 0)
	{
		return nullptr;
	}

	float Roll = FMath::FRand() * TotalWeight;
	for (int32 i = 0; i < Candidates.Num(); ++i)
	{
		Roll -= Weights[i];
		if (Roll <= 0.f)
		{
			return Candidates[i];
		}
	}

	return Candidates.Last();
}

void URSZombieDirectorSubsystem::AssignTarget(APawn* Zombie, APlayerState* Player)
{
	ensure(Player);

	if (!ZombiesByPlayer.Contains(Player))
	{
		ZombiesByPlayer.Add(Player);
	}
	ZombiesByPlayer[Player].Add(Zombie);
}

int32 URSZombieDirectorSubsystem::GetAssignedCountForPlayer(APlayerState* Player) const
{
	if (!ZombiesByPlayer.Contains(Player))
	{
		return 0;
	}

	return ZombiesByPlayer[Player].Num();
}
