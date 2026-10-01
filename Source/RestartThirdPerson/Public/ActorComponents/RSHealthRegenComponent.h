// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSHealthRegenComponent.generated.h"


class URSActionSystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API URSHealthRegenComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Health restored every second */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Regen")
	float RegenRate = 20.f;

	/** Time until regeneration after taking damage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Regen")
	float RegenDelay = 8.f;

protected:
	/** Callback to health changed event */
	void OnHealthChanged(float NewHealth, float OldHealth, AController* EventInstigator, AActor* InstigatorActor);

	/** Callback to health max changed event */
	void OnHealthMaxChanged(float NewHealthMax, float OldHealthMax, AController* EventInstigator, AActor* InstigatorActor);

protected:
	/** Callback to start regenerating health. Called RegenDelay seconds after being damaged */
	void StartRegen();

protected:
	/** Timer handle responsible for regenerating health after taking damage */
	FTimerHandle TimerHandle_RegenDelay;

	/** Cache reference to owner's ActionSystemComponent */
	UPROPERTY(Transient)
	TObjectPtr<URSActionSystemComponent> ActionSystemComponent;

public:
	/** Constructor */
	URSHealthRegenComponent();

	/** Initializes component. Called during level startup or actor spawn before BeginPlay */
	virtual void InitializeComponent() override;

	/** Called every frame */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
