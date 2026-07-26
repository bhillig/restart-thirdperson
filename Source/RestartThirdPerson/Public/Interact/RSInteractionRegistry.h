// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RSInteractionRegistry.generated.h"

class URSInteractableComponent;

/**
 *  Subsystem for holding all interactable components in the world.
 *  
 *  Interactable components register themselves here on BeginPlay.
 *  Players query for this registry's interactables when interacting.
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSInteractionRegistry : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	/** Registers an interaction component to the registry */
	void Register(URSInteractableComponent* InteractableComp);

	/** Unregisters an interaction component from the registry */
	void Unregister(URSInteractableComponent* InteractableComp);

	/** Returns all interactable components registered */
	const TArray<TObjectPtr<URSInteractableComponent>>& GetInteractables() const;

	/** Skip editor-preview / thumbnail worlds */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	/** Array of all registered interactables */
	UPROPERTY()
	TArray<TObjectPtr<URSInteractableComponent>> InteractableComps;
};
