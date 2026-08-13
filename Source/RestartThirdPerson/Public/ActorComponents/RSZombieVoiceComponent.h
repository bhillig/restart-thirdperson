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

	/** Enables all audio */
	void Enable();

	/** Disables all audio from playing */
	void Disable();

protected:
	/** Plays a chase sound. Called every ChaseInterval */
	UFUNCTION()
	void PlayChaseSound();

protected:
	/** Zombie grunt sound while in the chase state */
	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> ChaseZombieSound;

	/** Interval between grunt sounds while in the chase state */
	UPROPERTY(EditAnywhere, Category = "Config")
	float ChaseInterval = 5.f;

	FTimerHandle TimerHandle_ChaseSound;
};
