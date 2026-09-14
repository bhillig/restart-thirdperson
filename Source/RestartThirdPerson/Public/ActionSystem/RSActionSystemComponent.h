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

	/** Request to stop the action with a given name */
	void StopAction(FName InActionName);

protected:
	/** Default actions to grant upon initialization */
	UPROPERTY(EditAnywhere, Category="Actions")
	TArray<TSubclassOf<URSAction>> DefaultActions;

protected:
	/** Array of actions */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URSAction>> Actions;
};
