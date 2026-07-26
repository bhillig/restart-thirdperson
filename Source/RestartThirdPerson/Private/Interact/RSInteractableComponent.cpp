// Brandon Hillig 2026


#include "Interact/RSInteractableComponent.h"

#include "Interact/RSInteractionRegistry.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

URSInteractableComponent::URSInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool URSInteractableComponent::CanInteract(ARSPlayerState* PlayerState) const
{
	return true;
}

FRSInteractionPrompt URSInteractableComponent::GetInteractionPrompt(ARSPlayerState* PlayerState) const
{
	FRSInteractionPrompt Prompt;
	Prompt.Text = FText::FromString("Interact");
	return Prompt;
}

void URSInteractableComponent::Interact(ARSPlayerState* PlayerState)
{
	rs::LogOnce("Interacted!");
}

void URSInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register interactable component
	if (URSInteractionRegistry* InteractionRegistry = GetWorld()->GetSubsystem<URSInteractionRegistry>())
	{
		InteractionRegistry->Register(this);
	}
}

void URSInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister interactable component
	if (URSInteractionRegistry* InteractionRegistry = GetWorld()->GetSubsystem<URSInteractionRegistry>())
	{
		InteractionRegistry->Unregister(this);
	}

	Super::EndPlay(EndPlayReason);
}
