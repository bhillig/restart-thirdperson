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

	virtual void Apply(AActor* Owner, ARSPlayerState* Buyer) const override;

	virtual FString GetPromptText(ARSPlayerState* PlayerState) const override;

	virtual ERSPurchaseRepeatPolicy GetRepeatPolicy() const override;
};
