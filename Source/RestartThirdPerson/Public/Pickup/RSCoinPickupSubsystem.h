// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSCoinPickupSubsystem.generated.h"

UCLASS(Config = Game, DefaultConfig, DisplayName="Coin Pickup Subsystem Settings")
class URSCoinPickupSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Pickup Coin Mesh */
	UPROPERTY(Config, EditDefaultsOnly, Category="Pickups")
	TSoftObjectPtr<UStaticMesh> CoinMesh;

	/** Pickup Coin Sound */
	UPROPERTY(Config, EditDefaultsOnly, Category="Pickups")
	TSoftObjectPtr<USoundBase> CoinPickupSound;

	/** Pickup Trigger Name */
	UPROPERTY(Config, EditDefaultsOnly, Category="Pickups")
	FName CoinPickupTriggerParameter;

	virtual FName GetCategoryName() const override
	{
		return FApp::GetProjectName();
	}
};

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
	/** Removes coin pickups at the desired indices from the world */
	void RemoveCoinPickups(TArray<int32> IndicesToRemove);

protected:
	/** Location of all coins */
	TArray<FVector> CoinLocations;

	/** Points of all coins */
	TArray<int32> CoinPoints;

	/** Instance IDs of all coins */
	TArray<FPrimitiveInstanceId> CoinInstanceIDs;

protected:
	/** World Instanced Static Mesh Component */
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> WorldISM;

	/** World audio component to play coin pickup sounds */
	UPROPERTY()
	TObjectPtr<UAudioComponent> WorldAudioComponent;

protected:
	/** Cached parameter from Developer Settings for picking up coins */
	FName CoinPickedUpParameter;

protected:
	/** Plays/updates the coin pickup sound */
	void UpdatePickupSound();

protected:
	/** Callback for when the coin mesh is loaded */
	void OnCoinMeshLoaded(const FSoftObjectPath& ObjectPath, UObject* Object);

	/** Callback for when the coin pickup sound is loaded */
	void OnAudioLoaded(const FSoftObjectPath& ObjectPath, UObject* Object);

public:
	// UWorldSubsystem implementation Begin
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// UWorldSubsystem implementation End

	// FTickableGameObject implementation Begin
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// FTickableGameObject implementation End
};
