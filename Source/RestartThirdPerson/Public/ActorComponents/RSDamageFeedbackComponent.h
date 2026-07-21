// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSDamageFeedbackComponent.generated.h"

/**
 *  Damage Feedback info to fill when broadcasting the DamageFeedback delegate
 */
USTRUCT(BlueprintType)
struct FRSDamageFeedbackEvent
{
	GENERATED_BODY()

	/** Damage applied */
	UPROPERTY(BlueprintReadOnly, Category = "Feedback")
	float Damage = 0.f;

	/** Signed yaw to the damage source, relative to camera forward */
	UPROPERTY(BlueprintReadOnly, Category = "Feedback")
	float DirectionAngle = 0.f;

	/** Whether this damage was lethal */
	UPROPERTY(BlueprintReadOnly, Category = "Feedback")
	bool bWasLethal = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnDamageFeedbackDelegate, const FRSDamageFeedbackEvent&, DamageFeedback);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API URSDamageFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSDamageFeedbackComponent();

protected:
	/** Called to initialize the component */
	virtual void InitializeComponent() override;	

	/** Called when the owning actor takes point damage. Called before OnHealthChanged. */
	UFUNCTION()
	void OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	/** Called when the owning actor's health changes. Requires Owner to have UAttributeComponent. */
	UFUNCTION()
	void OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser);

public:
	/** Damage feedback delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnDamageFeedbackDelegate OnDamageFeedback;

protected:
	/** Camera shake to apply when receiving damage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	TSubclassOf<UCameraShakeBase> DamageCameraShake;

protected:
	/** Last direction we were damaged in. Cached in OnTakePointDamage and used in OnHealthChanged. */
	FVector ShotFromDirectionLast = FVector::ZeroVector;

	/** Whether we received a direction from OnTakePointDamage and should use it in OnHealthChanged. Set back to false after using. */
	bool bUseDirection = false;
};
