// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSPurchaseEffect.h"
#include "RSPurchaseEffect_GrantWeapon.generated.h"

class UWeaponDataAsset;

/**
 *  Purchase effect that grants the player a weapon
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Grant Weapon"))
struct FRSPurchaseEffect_GrantWeapon : public FRSPurchaseEffect
{
	GENERATED_BODY()

	/** The weapon this effect grants */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UWeaponDataAsset> WeaponData;

	virtual void Apply(AActor* Owner, ARSPlayerState* Buyer) const override;

	virtual FString GetPromptText() const override;

	virtual ERSPurchaseRepeatPolicy GetRepeatPolicy() const override;
};
