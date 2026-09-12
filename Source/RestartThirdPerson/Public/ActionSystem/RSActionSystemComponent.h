// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RSActionSystemComponent.generated.h"

class URSAction;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	URSActionSystemComponent();

	/** Called when initializing the component */
	virtual void InitializeComponent() override;

	/** Request to start the action with a given name */
	void StartAction(FName InActionName);

protected:
	/** Array of actions */
	UPROPERTY()
	TArray<TObjectPtr<URSAction>> Actions;
};
