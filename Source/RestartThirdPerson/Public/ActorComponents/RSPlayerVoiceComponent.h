// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RSPlayerVoiceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API URSPlayerVoiceComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSPlayerVoiceComponent();

	/** Initializes component and binds delegates */
	virtual void InitializeComponent() override;

protected:
	/** Callback for when the owning player's health changes */
	UFUNCTION()
	void OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser);

	/** Callback for when the owning player dies */
	UFUNCTION()
	void OnDeath(AController* EventInstigator, AActor* DamageCauser);

	/** Hit react sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> HitReactSound;

	/** Death sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> DeathSound;

protected:
	/** Server -> All: Notify clients to spawn a sound attached to this */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnSoundAttached(USoundBase* Sound);
};
