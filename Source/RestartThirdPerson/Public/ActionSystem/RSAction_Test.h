// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "RSAction.h"
#include "RSAction_Test.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Test);

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
