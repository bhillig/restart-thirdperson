// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSZombieDirectorSubsystem.generated.h"

class AZombieCharacter;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSZombieDirectorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** Determines whether this subsystem supports this world type */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Called when the world begins */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Spawns a zombie and sets its target */
	AZombieCharacter* SpawnZombie(TSubclassOf<AZombieCharacter> ZombieClass);

	APawn* GetPlayerTarget(APawn* Zombie);

protected:
	/** Determines who should be targeted by a new zombie */
	APlayerState* DetermineTarget() const;

	/** Assigns a target to the zombie */
	void AssignTarget(APawn* Zombie, APlayerState* Target);

protected:
	/** Map of players and the zombies currently targeting them */
	TMap<TWeakObjectPtr<APlayerState>, TSet<TWeakObjectPtr<APawn>>> ZombiesByPlayer;

	/** Array of target spawn points. Filled in on BeginPlay */
	UPROPERTY()
	TArray<AActor*> SpawnPoints;
};
