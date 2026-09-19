// Brandon Hillig 2026


#include "ActionSystem/RSActionSystemComponent.h"

#include "ActionSystem/RSAction.h"
#include "ActionSystem/RSAttributeSet.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

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

	rs::LogOnce(FString::Printf(TEXT("Could not start action: %s. The tag was not found in available actions!"),
		*InActionTag.ToString()), FColor::Yellow, 5.0f);
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

	rs::LogOnce(FString::Printf(TEXT("Could not stop action: %s. The tag was not found in available actions!"),
		*InActionTag.ToString()), FColor::Yellow, 5.0f);
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
	case Base:
		Attribute->Base += Delta;
		break;
	case Modifier:
		Attribute->Modifier += Delta;
		break;
	case BaseOverride:
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
		rs::LogOnce(Msg);
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
