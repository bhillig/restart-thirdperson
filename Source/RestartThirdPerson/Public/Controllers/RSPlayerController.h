// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RSPlayerController.generated.h"

class AALSCharacter;
class ARSPlayerState;
class ARSZombiesGameState;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API ARSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/** Called when the game begins */
	virtual void BeginPlay() override;

	/** Called when we possess a pawn (Server and clients) */
	virtual void AcknowledgePossession(APawn* ) override;

	/** Called when the player state replicates */
	virtual void OnRep_PlayerState() override;

protected:
	/** Creates user widgets (Blueprints) */
	UFUNCTION(BlueprintImplementableEvent)
	void CreateUserWidgets();

	/** Binds callbacks to the game state's delegates (Blueprints) */
	UFUNCTION(BlueprintImplementableEvent)
	void BindToGameState(ARSZombiesGameState* ZombiesGameState);

	/** Binds callbacks to the player character delegates (Blueprints) */
	UFUNCTION(BlueprintImplementableEvent)
	void BindToPlayerCharacter(AALSCharacter* PlayerCharacter);

	/** Binds callbacks to the player state delegates (Blueprints) */
	UFUNCTION(BlueprintImplementableEvent)
	void BindToPlayerState(ARSPlayerState* RSPlayerState);
};
