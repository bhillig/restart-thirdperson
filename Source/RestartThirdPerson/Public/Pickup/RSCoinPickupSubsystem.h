// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSCoinPickupSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSCoinPickupSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** Adds coin pickups to the system */
	UFUNCTION(BlueprintCallable)
	void AddCoinPickups(TArray<FVector> Locations, TArray<int32> Points);

protected:
	/** Location of all coins */
	TArray<FVector> CoinLocations;

	/** Points of all coins */
	TArray<int32> CoinPoints;

public:
	// FTickableGameObject implementation Begin
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// FTickableGameObject implementation End
};
