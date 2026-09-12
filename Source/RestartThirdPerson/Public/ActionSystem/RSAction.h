// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RSAction.generated.h"

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSAction : public UObject
{
	GENERATED_BODY()

public:
	/** Starts the action */
	UFUNCTION()
	void StartAction();

	/** Returns the name of the action */
	UFUNCTION(BlueprintPure, Category = "Action")
	FName GetActionName() const
	{
		return ActionName;
	}

protected:
	/** The name of the action */
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	FName ActionName = "TestAction";
	
};
