// Brandon Hillig 2026


#include "ActionSystem/RSAction.h"

#include "ActionSystem/RSActionEffect.h"
#include "ActionSystem/RSActionSystemComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction::StartAction_Implementation()
{
	bIsRunning = true;

	URSActionSystemComponent* ActionSystemComponent = GetOwningComponent();
	ensure(ActionSystemComponent);

	// Apply Activation Cost
	for (const auto& [AttributeTag, Cost] : ActivationCost)
	{
		ActionSystemComponent->ApplyAttributeChange(AttributeTag, -Cost, EAttributeChangeType::Base);
	}

	// Apply Action Effects
	for (TSubclassOf<URSActionEffect> ActionEffectClass : ActionEffectClasses)
	{
		if (URSActionEffect* ActionEffect = ActionSystemComponent->ApplyActionEffect(ActionEffectClass))
		{
			// Track action effects applied with an action based duration so we can remove them on stop action
			if (ActionEffect->GetEffectType() == EEffectType::ActionDuration)
			{
				ActionEffectsApplied.Add(ActionEffect);
			}
		}
	}
}

void URSAction::StopAction_Implementation()
{
	bIsRunning = false;

	URSActionSystemComponent* ActionSystemComponent = GetOwningComponent();
	ensure(ActionSystemComponent);

	// Remove Action Effects
	for (URSActionEffect* ActionEffect : ActionEffectsApplied)
	{
		ActionSystemComponent->RemoveActionEffect(ActionEffect);
	}
}

bool URSAction::CanStart() const
{
	if (IsRunning())
	{
		rs::LogOnce("Can't start action. It's already running!", FColor::Red, 3.0f);
		return false;
	}

	if (GetTimeUntilCooldownExpires() > 0.f)
	{
		rs::LogOnce("Can't start action. Cooldown isn't over!", FColor::Red, 3.0f);
		return false;
	}

	URSActionSystemComponent* ActionSystemComponent = GetOwningComponent();
	ensure(ActionSystemComponent);

	if (ActionSystemComponent->HasAnyGameplayTagFrom(BlockedTags))
	{
		rs::LogOnce("Can't start action. Instigator has blocked tags!", FColor::Red, 3.0f);
		return false;
	}

	for (const auto&[AttributeTag, Cost] : ActivationCost)
	{
		const float AttributeAmount = ActionSystemComponent->GetAttributeValue(AttributeTag);
		if (AttributeAmount < Cost)
		{
			const FString Msg = FString::Printf(TEXT("Can't start action. Action requires: %f of %s. Pawn has: %f"), Cost, *AttributeTag.ToString(), AttributeAmount);
			rs::LogOnce("", FColor::Red, 3.0f);
			return false;
		}
	}

	return true;
}

float URSAction::GetTimeUntilCooldownExpires() const
{
	return FMath::Max(0.f, GameTimeActionBecomesActive - GetWorld()->GetTimeSeconds());
}

URSActionSystemComponent* URSAction::GetOwningComponent() const
{
	return Cast<URSActionSystemComponent>(GetOuter());
}
