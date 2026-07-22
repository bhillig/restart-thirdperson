// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RSZombiesGameMode.generated.h"

class AZombieCharacter;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API ARSZombiesGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	/** Called when the simulation has started */
	virtual void BeginPlay() override;

	/** Returns the number of zombies to spawn for a given round number and player count */
	int32 GetZombieCountForRound(int32 RoundNumber, int32 PlayerCount);

	/** Returns the max number of concurrent zombies active for a given player count */
	int32 GetConcurrentZombieCap(int32 PlayerCount);

protected:
	/** Zombie class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Zombie")
	TSubclassOf<AZombieCharacter> ZombieClass;

protected:
	UPROPERTY(EditAnywhere)
	int32 CurrentRoundNumber = 1;
};
