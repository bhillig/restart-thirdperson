// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSGameAudioSubsystem.generated.h"

UCLASS(BlueprintType, meta=(DisplayName = "Game Audio Catalog"))
class URSGameAudioCatalogData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> RoundStarted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> RoundCompleted;
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Game Audio Subsystem Settings"))
class URSGameAudioSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<URSGameAudioCatalogData> GameAudioCatalog;
};

/**
 *  Subsystem for playing audio in response to game events
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSGameAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Skip editor-preview / thumbnail worlds */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Called when the game begins */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	/** Binds delegates to the game state */
	UFUNCTION()
	void BindGameState(AGameStateBase* GameState);

	/** State of whether we are currently bounded to delegates */
	bool bBound = false;

protected:
	/** Cache reference to Game Audio Catalog Data. Loaded in OnWorldBeginPlay */
	UPROPERTY(Transient)
	TObjectPtr<URSGameAudioCatalogData> GameAudioCatalog = nullptr;

	/** Callback for when a new zombies round starts */
	UFUNCTION()
	void HandleRoundStarted(int32 RoundNumber);

	/** Callback for when a zombies round completes */
	UFUNCTION()
	void HandleRoundCompleted(int32 RoundNumber);


};
