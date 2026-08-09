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
	/** Initialize the component and bind delegates */
	virtual void InitializeComponent() override;

	/** Called when the game begins */
	virtual void BeginPlay() override;

protected:
	/** Callback for when the owning zombie dies */
	UFUNCTION()
	void OnDeath(AController* EventInstigator, AActor* DamageCauser);

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
