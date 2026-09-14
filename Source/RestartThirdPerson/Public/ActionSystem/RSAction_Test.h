// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSAction.h"
#include "UObject/Object.h"
#include "RSAction_Test.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class RESTARTTHIRDPERSON_API URSAction_Test : public URSAction
{
	GENERATED_BODY()

public:
	/** URSAction Interface */
	virtual void StartAction_Implementation() override;
	/** URSAction Interface */
};
