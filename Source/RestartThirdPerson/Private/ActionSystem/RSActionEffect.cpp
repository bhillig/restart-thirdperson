// Brandon Hillig 2026

#include "ActionSystem/RSActionEffect.h"

#include "ActionSystem/RSActionSystemComponent.h"

void URSActionEffect::OnApplyEffect()
{
	URSActionSystemComponent* OwningComponent = GetOwningComponent();
	ensure(OwningComponent);

	// Apply tags
	OwningComponent->ActiveGameplayTags.AppendTags(GrantedTags);

	// Set timer for duration based effects
	if (EffectType == EEffectType::Duration)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, FTimerDelegate::CreateUObject(OwningComponent, &URSActionSystemComponent::RemoveStatusEffect, ActionEffectTag), Duration, false);
	}

	// Call blueprint event
	BlueprintOnApplyEffect();
}

void URSActionEffect::OnRemoveEffect()
{
	URSActionSystemComponent* OwningComponent = GetOwningComponent();
	ensure(OwningComponent);

	// Remove tags
	OwningComponent->ActiveGameplayTags.RemoveTags(GrantedTags);

	// Call blueprint event
	BlueprintOnRemoveEffect();
}

URSActionSystemComponent* URSActionEffect::GetOwningComponent() const
{
	return Cast<URSActionSystemComponent>(GetOuter());
}
