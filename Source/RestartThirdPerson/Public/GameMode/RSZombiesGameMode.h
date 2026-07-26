// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RSZombiesGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnRoundStartedDelegate, int32, RoundNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnRoundCompletedDelegate, int32, RoundNumber);

class ATargetPoint;
class AZombieCharacter;

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API ARSZombiesGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	ARSZombiesGameMode();

protected:
	/** Called when the simulation has started */
	virtual void BeginPlay() override;

	/** Called when the new round is beginning */
	void AdvanceRound();

	/** Called when the player(s) have killed all the zombies in the current round */
	void CompleteRound();

	/** Returns the number of zombies to spawn for a given round number and player count */
	int32 GetZombieCountForRound(int32 RoundNumber, int32 PlayerCount);

	/** Returns the max number of concurrent zombies active for a given player count */
	int32 GetConcurrentZombieCap(int32 PlayerCount);

	/** Tries to spawn a zombie if conditions are met */
	void TrySpawnZombie();

public:
	/** On Round Started Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnRoundStartedDelegate OnRoundStarted;

	/** On Round Completed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnRoundCompletedDelegate OnRoundCompleted;

protected:
	/** Called when a zombie is hit/shot */
	UFUNCTION()
	void OnZombieHit(AController* InstigatedBy);

	/** Called when a zombie dies */
	UFUNCTION()
	void OnZombieDeath(AController* InstigatedBy, bool bWasHeadshot);

protected:
	/** SPAWN RULES */

	/** Zombie class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Rules|Spawn")
	TSubclassOf<AZombieCharacter> ZombieClass;

	/** Interval between zombie spawns */
	UPROPERTY(EditDefaultsOnly, Category = "Rules|Spawn")
	float SpawnInterval = 2.f;

	/** GAME RULES */

	/** Duration between rounds */
	UPROPERTY(EditDefaultsOnly, Category = "Rules|Game")
	float RoundBreakDuration = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Rules|Credits")
	int32 CreditsAwardedPerHit = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Rules|Credits")
	int32 CreditsAwardedPerKill = 60;

	UPROPERTY(EditDefaultsOnly, Category = "Rules|Credits")
	int32 CreditsAwardedPerHeadshotKill = 100;

	/** Array of target spawn points. Filled in on BeginPlay */
	UPROPERTY()
	TArray<AActor*> SpawnPoints;

protected:
	UPROPERTY()
	int32 CurrentRoundNumber = 0;

	UPROPERTY()
	int32 ZombiesLeftToSpawnThisRound = 0;

	UPROPERTY()
	int32 ZombiesAlive = 0;

	UPROPERTY()
	FTimerHandle TimerHandle_ZombieSpawn;

	UPROPERTY()
	FTimerHandle TimerHandle_RoundBreak;
};
