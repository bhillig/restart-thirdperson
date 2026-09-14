// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RSAction.generated.h"

class URSActionSystemComponent;
/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class RESTARTTHIRDPERSON_API URSAction : public UObject
{
	GENERATED_BODY()

public:
	/** Starts the action */
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction();

	/** Stops the action */
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StopAction();

	/** Returns the name of the action */
	UFUNCTION(BlueprintPure, Category="Action")
	FName GetActionName() const
	{
		return ActionName;
	}

	/** Returns the owning action system component this ability is within */
	UFUNCTION(BlueprintPure, Category="Action")
	URSActionSystemComponent* GetOwningComponent() const;

protected:
	/** The name of the action */
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FName ActionName = "Action";
	
};
