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

	for (TSubclassOf<URSAttributeSet> AttributeSetClass : AttributeSetClasses)
	{
		AttributeSets.Add(NewObject<URSAttributeSet>(this, AttributeSetClass));
	}

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

	if (FOnAttributeChanged* AttributeDelegate = AttributeDelegates.Find(InAttributeTag))
	{
		AttributeDelegate->Broadcast(Attribute->GetValue(), OldValue, EventInstigator, InstigatorActor);
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

FRSAttribute* URSActionSystemComponent::FindAttributeByTag(FGameplayTag InAttributeTag)
{
	if (FRSAttribute** FoundAttribute = CachedAttributes.Find(InAttributeTag))
	{
		return *FoundAttribute;
	}
	return nullptr;
}

URSAttributeSet* URSActionSystemComponent::FindOwningAttributeSet(FRSAttribute* Attribute)
{
	if (URSAttributeSet** FoundAttributeSet = CachedAttributeSets.Find(Attribute))
	{
		return *FoundAttributeSet;
	}
	return nullptr;
}

FOnAttributeChanged& URSActionSystemComponent::GetAttributeDelegate(FGameplayTag InAttributeTag)
{
	return AttributeDelegates.FindOrAdd(InAttributeTag);
}
