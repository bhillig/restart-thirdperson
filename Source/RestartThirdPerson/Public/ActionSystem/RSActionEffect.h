// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RSActionEffect.generated.h"

class URSActionSystemComponent;

UENUM(BlueprintType, DisplayName="Effect Type")
enum class EEffectType : uint8
{
	Manual,
	Duration
};

/**
 *  Action Effect
 */
UCLASS(Abstract, Blueprintable)
class RESTARTTHIRDPERSON_API URSActionEffect : public UObject
{
GENERATED_BODY()

public:
	/** Apply action effect, called by ActionSystemComponent */
	virtual void OnApplyEffect();

	/** Called when an action effect is being removed, called by ActionSystemComponent or by timer (if duration is selected) */
	virtual void OnRemoveEffect();

	/** Apply action effect (Blueprint classes implement this) */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Apply Effect", Category="Effect")
	void BlueprintOnApplyEffect();

	/** Remove action effect (Blueprint classes implement this) */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Remove Effect", Category="Effect")
	void BlueprintOnRemoveEffect();

	UFUNCTION(BlueprintPure, Category="Effect")
	FGameplayTag GetActionEffectTag() const
	{
		return ActionEffectTag;
	}

protected:
	/** Action effect identifier */
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag ActionEffectTag;

	/** Type of action effect */
	UPROPERTY(EditAnywhere, Category="Effect")
	EEffectType EffectType;

	/** Duration of effect in seconds (only applicable if EffectType is set to EEffectType::Duration) */
	UPROPERTY(EditAnywhere, Category="Effect", meta = (EditCondition="EffectType == EEffectType::Duration", EditConditionHides))
	float Duration = 0.f;

	/** Gameplay tags applied while the effect is active */
	UPROPERTY(EditAnywhere, Category="Effect")
	FGameplayTagContainer GrantedTags;

protected:
	UFUNCTION(BlueprintPure)
	URSActionSystemComponent* GetOwningComponent() const;

private:
	/** Timer handle used for duration based effects */
	FTimerHandle TimerHandle_Duration;
};
