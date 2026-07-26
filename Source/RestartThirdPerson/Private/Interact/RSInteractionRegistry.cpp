// Brandon Hillig 2026


#include "Interact/RSInteractionRegistry.h"

void URSInteractionRegistry::Register(URSInteractableComponent* InteractableComp)
{
	InteractableComps.AddUnique(InteractableComp);
}

void URSInteractionRegistry::Unregister(URSInteractableComponent* InteractableComp)
{
	InteractableComps.RemoveSingleSwap(InteractableComp);
}

const TArray<TObjectPtr<URSInteractableComponent>>& URSInteractionRegistry::GetInteractables() const
{
	return InteractableComps;
}

bool URSInteractionRegistry::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Only register interactables when playing
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}
