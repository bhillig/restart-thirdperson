// Brandon Hillig 2026


#include "ActionSystem/RSActionSystemComponent.h"

#include "ActionSystem/RSAction.h"
#include "ActionSystem/RSActionEffect.h"
#include "ActionSystem/RSAttributeSet.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

static TAutoConsoleVariable<bool> CVarDebugActions(TEXT("Game.DebugActions"), false, TEXT("Enables logging of actions and action effects"));
static TAutoConsoleVariable<bool> CVarDebugAttributes(TEXT("Game.DebugAttributes"), false, TEXT("Enables logging of attribute changes"));

URSActionSystemComponent::URSActionSystemComponent()
{
	bWantsInitializeComponent = true;
}

void URSActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (URSAttributeSet* AttributeSet : AttributeSets)
	{
		for (TFieldIterator<FStructProperty> PropIt(AttributeSet->GetClass()); PropIt; ++PropIt)
		{
			FRSAttribute* FoundAttribute = PropIt->ContainerPtrToValuePtr<FRSAttribute>(AttributeSet);

			FName AttributeTagName = FName("Attribute." + PropIt->GetName());
			FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);

			CachedAttributes.Add(AttributeTag, FoundAttribute);
			CachedAttributeSets.Add(FoundAttribute, AttributeSet);
		}
	}
	

	for (TSubclassOf<URSAction> ActionClass : DefaultActions)
	{
		if (ensure(ActionClass))
		{
			Actions.Add(NewObject<URSAction>(this, ActionClass));
		}
	}
}

void URSActionSystemComponent::StartAction(FGameplayTag InActionTag)
{
	for (URSAction* Action : Actions)
	{
		if (InActionTag.MatchesTagExact(Action->GetActionTag()))
		{
			if (Action->CanStart())
			{
				Action->StartAction();
			}
			return;
		}
	}

#if !UE_BUILD_SHIPPING
	if (CVarDebugActions.GetValueOnGameThread())
	{
		rs::LogOnce(FString::Printf(TEXT("Could not start action: %s. The tag was not found in available actions!"),
			*InActionTag.ToString()), FColor::Yellow, 5.0f);
	}
#endif 
}

void URSActionSystemComponent::StopAction(FGameplayTag InActionTag)
{
	for (URSAction* Action : Actions)
	{
		if (InActionTag.MatchesTagExact(Action->GetActionTag()))
		{
			if (Action->IsRunning())
			{
				Action->StopAction();
			}
			return;
		}
	}

#if !UE_BUILD_SHIPPING
	if (CVarDebugActions.GetValueOnGameThread())
	{
		rs::LogOnce(FString::Printf(TEXT("Could not stop action: %s. The tag was not found in available actions!"),
			*InActionTag.ToString()), FColor::Yellow, 5.0f);
	}
#endif 
}

bool URSActionSystemComponent::ApplyAttributeChange(FGameplayTag InAttributeTag, float Delta, EAttributeChangeType ChangeType, AController* EventInstigator, AActor* InstigatorActor)
{
	FRSAttribute* Attribute = FindAttributeByTag(InAttributeTag);
	if (!Attribute)
	{
		rs::LogOnce(FString::Printf(TEXT("Could not apply attribute change to attribute: %s. Name not found!"), *InAttributeTag.ToString()),
			FColor::Red, 3.0f);
		return false;
	}

	const float OldValue = Attribute->GetValue();

	switch (ChangeType)
	{
	case EAttributeChangeType::Base:
		Attribute->Base += Delta;
		break;
	case EAttributeChangeType::Modifier:
		Attribute->Modifier += Delta;
		break;
	case EAttributeChangeType::BaseOverride:
		Attribute->Base = Delta;
		break;
	default:
		check(false);
	}

	URSAttributeSet* OwningAttributeSet = FindOwningAttributeSet(Attribute);
	ensure(OwningAttributeSet);

	OwningAttributeSet->PostAttributeChange();

	// Broadcast to Native Listeners
	if (FOnAttributeChanged* AttributeListener = AttributeListeners.Find(InAttributeTag))
	{
		AttributeListener->Broadcast(Attribute->GetValue(), OldValue, EventInstigator, InstigatorActor);
	}

	// Broadcast to Blueprint Listeners
	if (TArray<FOnAttributeChangedDynamic>* BlueprintListenersArray = BlueprintAttributeListeners.Find(InAttributeTag))
	{
		TArray<FOnAttributeChangedDynamic>& BlueprintListeners = *BlueprintListenersArray;
		for (int32 i = BlueprintListeners.Num() - 1; i >= 0; --i)
		{
			FOnAttributeChangedDynamic& AttributeListener = BlueprintListeners[i];
			if (!AttributeListener.ExecuteIfBound(Attribute->GetValue(), OldValue, EventInstigator, InstigatorActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("Cleaned up unbound blueprint attribute listener for: %s"), *GetNameSafe(GetOwner()));
				// Remove unbound delegate
				BlueprintListeners.RemoveAt(i);
			}
		}
	}

#if !UE_BUILD_SHIPPING
	if (CVarDebugAttributes.GetValueOnGameThread())
	{
		const FString Msg = FString::Printf(TEXT("Attribute: %s, New Value: %f, Old Value: %f"), *InAttributeTag.ToString(), Attribute->GetValue(), OldValue);
		rs::LogOnce(Msg, FColor::Emerald, 5.f);
	}
#endif

	return true;
}

FRSAttribute* URSActionSystemComponent::FindAttributeByTag(FGameplayTag InAttributeTag) const
{
	if (FRSAttribute* const* FoundAttribute = CachedAttributes.Find(InAttributeTag))
	{
		return *FoundAttribute;
	}
	return nullptr;
}

float URSActionSystemComponent::GetAttributeValue(FGameplayTag InAttributeTag) const
{
	if (FRSAttribute* FoundAttribute = FindAttributeByTag(InAttributeTag))
	{
		return FoundAttribute->GetValue();
	}
	ensure(false);
	return 0.f;
}

URSActionEffect* URSActionSystemComponent::ApplyActionEffect(TSubclassOf<URSActionEffect> ActionEffectClass)
{
	URSActionEffect* ActionEffect = NewObject<URSActionEffect>(this, ActionEffectClass);
	ActionEffect->OnApplyEffect();
	ActionEffects.Add(ActionEffect);

#if !UE_BUILD_SHIPPING
	if (CVarDebugActions.GetValueOnGameThread())
	{
		const FString Msg = FString::Printf(TEXT("Applied action effect: %s on actor: %s"), *GetNameSafe(ActionEffect), *GetNameSafe(GetOwner()));
		rs::LogOnce(Msg, FColor::Emerald);
	}
#endif

	return ActionEffect;
}

void URSActionSystemComponent::RemoveActionEffect(URSActionEffect* ActionEffect)
{
	if (ActionEffect && ActionEffects.RemoveSingle(ActionEffect) > 0)
	{
		ActionEffect->OnRemoveEffect();

#if !UE_BUILD_SHIPPING
		if (CVarDebugActions.GetValueOnGameThread())
		{
			const FString Msg = FString::Printf(TEXT("Removed action effect: %s on actor: %s"), *GetNameSafe(ActionEffect), *GetNameSafe(GetOwner()));
			rs::LogOnce(Msg, FColor::Emerald);
		}
#endif
	}
}

void URSActionSystemComponent::RemoveActionEffectWithTag(FGameplayTag ActionEffectTag)
{
	TArray<URSActionEffect*> ActionEffectsRemoved;

	for (int32 i = ActionEffects.Num() - 1; i >= 0; --i)
	{
		URSActionEffect* Effect = ActionEffects[i];
		if (Effect->GetActionEffectTag() == ActionEffectTag)
		{
			ActionEffectsRemoved.Add(Effect);
			ActionEffects.RemoveAt(i);
		}
	}

	for (URSActionEffect* Effect : ActionEffectsRemoved)
	{
		Effect->OnRemoveEffect();

#if !UE_BUILD_SHIPPING
		if (CVarDebugActions.GetValueOnGameThread())
		{
			const FString Msg = FString::Printf(TEXT("Removed action effect: %s on actor: %s"), *GetNameSafe(Effect), *GetNameSafe(GetOwner()));
			rs::LogOnce(Msg, FColor::Emerald);
		}
#endif
	}
}

void URSActionSystemComponent::AppendGameplayTags(const FGameplayTagContainer& TagContainer)
{
	for (FGameplayTag Tag : TagContainer)
	{
		AddGameplayTag(Tag);
	}
}

void URSActionSystemComponent::AddGameplayTag(FGameplayTag Tag)
{
	ActiveGameplayTags.AddTag(Tag);

	// Broadcast to native listeners
	if (FOnGameplayTagAdded* OnTagAdded = NativeGameplayTagsAddedListeners.Find(Tag))
	{
		OnTagAdded->Broadcast();
	}
}

void URSActionSystemComponent::RemoveGameplayTags(const FGameplayTagContainer& TagContainer)
{
	for (FGameplayTag Tag : TagContainer)
	{
		RemoveGameplayTag(Tag);
	}
}

void URSActionSystemComponent::RemoveGameplayTag(FGameplayTag Tag)
{
	ActiveGameplayTags.RemoveTag(Tag);

	// Broadcast to native listeners
	if (FOnGameplayTagRemoved* OnTagRemoved = NativeGameplayTagsRemovedListeners.Find(Tag))
	{
		OnTagRemoved->Broadcast();
	}
}

bool URSActionSystemComponent::HasAnyGameplayTagFrom(const FGameplayTagContainer& TagContainer)
{
	return ActiveGameplayTags.HasAny(TagContainer);
}

bool URSActionSystemComponent::HasGameplayTag(FGameplayTag Tag)
{
	return ActiveGameplayTags.HasTag(Tag);
}

URSAttributeSet* URSActionSystemComponent::FindOwningAttributeSet(FRSAttribute* Attribute) const
{
	if (URSAttributeSet* const* FoundAttributeSet = CachedAttributeSets.Find(Attribute))
	{
		return *FoundAttributeSet;
	}
	return nullptr;
}

FOnAttributeChanged& URSActionSystemComponent::GetAttributeListener(FGameplayTag InAttributeTag)
{
	return AttributeListeners.FindOrAdd(InAttributeTag);
}

void URSActionSystemComponent::AddDynamicAttributeListener(FGameplayTag InAttributeTag, FOnAttributeChangedDynamic Event)
{
	TArray<FOnAttributeChangedDynamic>& BlueprintListeners = BlueprintAttributeListeners.FindOrAdd(InAttributeTag);
	BlueprintListeners.Add(Event);
}

void URSActionSystemComponent::RemoveDynamicAttributeListener(FOnAttributeChangedDynamic Event)
{
	for (TPair<FGameplayTag, TArray<FOnAttributeChangedDynamic>> Pair : BlueprintAttributeListeners)
	{
		if (Pair.Value.RemoveSingle(Event) > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Removed blueprint attribute listener for: %s"), *GetNameSafe(GetOwner()));
			break;
		}
	}
}

FOnGameplayTagAdded& URSActionSystemComponent::GetGameplayTagAddedListener(FGameplayTag Tag)
{
	return NativeGameplayTagsAddedListeners.FindOrAdd(Tag);
}

FOnGameplayTagRemoved& URSActionSystemComponent::GetGameplayTagRemovedListener(FGameplayTag Tag)
{
	return NativeGameplayTagsRemovedListeners.FindOrAdd(Tag);
}
