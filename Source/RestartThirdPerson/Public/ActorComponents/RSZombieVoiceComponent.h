// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSZombieVoiceComponent.generated.h"

/**
 *  Responsible for playing audio on the zombie when chasing
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API URSZombieVoiceComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSZombieVoiceComponent();

protected:
	/** Called when the game begins */
	virtual void BeginPlay() override;

protected:
	/** Plays a chase sound. Called every ChaseInterval */
	UFUNCTION()
	void PlayChaseSound();

protected:
	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> ChaseZombieSound;

	UPROPERTY(EditAnywhere, Category = "Config")
	float ChaseInterval = 5.f;


	FTimerHandle TimerHandle_ChaseSound;
};
