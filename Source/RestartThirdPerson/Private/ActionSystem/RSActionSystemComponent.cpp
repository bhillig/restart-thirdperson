// Brandon Hillig 2026


#include "ActionSystem/RSActionSystemComponent.h"

#include "ActionSystem/RSAction.h"
#include "ActionSystem/RSAttributeSet.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

URSActionSystemComponent::URSActionSystemComponent()
{
	bWantsInitializeComponent = true;
	AttributeSetClass = URSAttributeSet::StaticClass();
}

void URSActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Attributes = NewObject<URSAttributeSet>(this, AttributeSetClass);

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
