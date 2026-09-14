// Brandon Hillig 2026


#include "ActionSystem/RSActionSystemComponent.h"

#include "ActionSystem/RSAction.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

URSActionSystemComponent::URSActionSystemComponent()
{
	bWantsInitializeComponent = true;
}

void URSActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (TSubclassOf<URSAction> ActionClass : DefaultActions)
	{
		if (ensure(ActionClass))
		{
			Actions.Add(NewObject<URSAction>(this, ActionClass));
		}
	}
}

void URSActionSystemComponent::StartAction(FName InActionName)
{
	for (URSAction* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			Action->StartAction();
			return;
		}
	}

	rs::LogOnce(FString::Printf(TEXT("Could not start action: %s. The name was not found in available actions!"),
		*InActionName.ToString()), FColor::Yellow, 5.0f);
}

void URSActionSystemComponent::StopAction(FName InActionName)
{
	for (URSAction* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			Action->StopAction();
			return;
		}
	}

	rs::LogOnce(FString::Printf(TEXT("Could not stop action: %s. The name was not found in available actions!"),
		*InActionName.ToString()), FColor::Yellow, 5.0f);
}
