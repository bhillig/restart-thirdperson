// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSPurchaseEffect.h"
#include "RSPurchaseEffect_UnlockDoor.generated.h"

/**
 *  Purchase effect that unlocks a locked door
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Unlock Door"))
struct FRSPurchaseEffect_UnlockDoor : public FRSPurchaseEffect
{
	GENERATED_BODY()

	/** The door this effect unlocks */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> DoorActor;

	virtual void Apply(AActor* Owner, ARSPlayerState* Buyer) override;

	virtual ERSPurchaseRepeatPolicy GetRepeatPolicy() override;
};
