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
	// Get number of players
	const int32 NumberOfPlayers = GetWorld()->GetNumPlayerControllers();

	// If solo
	if (NumberOfPlayers == 1)
	{
		return GetWorld()->GetFirstPlayerController()->GetPlayerState<APlayerState>();
	}

	return nullptr;
}

void URSZombieDirectorSubsystem::AssignTarget(APawn* Zombie, APlayerState* Target)
{
	ensure(Target);

	if (!ZombiesByPlayer.Contains(Target))
	{
		ZombiesByPlayer.Add(Target);
	}
	ZombiesByPlayer[Target].Add(Zombie);
}
