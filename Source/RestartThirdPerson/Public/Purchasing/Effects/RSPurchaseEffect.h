// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "RSPurchaseEffect.generated.h"

class ARSPlayerState;

/**
 *  Purchase Effect Repeat Policy
 *  
 *  Determines whether a given effect can be repeatedly purchased.
 */
UENUM()
enum class ERSPurchaseRepeatPolicy : uint8
{
	Once,
	OncePerPlayer,
	Unlimited
};

/**
 *  Purchase Effect 
 */
USTRUCT(meta=(Hidden))
struct FRSPurchaseEffect
{
	GENERATED_BODY()
	virtual ~FRSPurchaseEffect() = default;

	/** Server-only. Runs once the credits have been spent */
	virtual void Apply(AActor* Owner, ARSPlayerState* Buyer) const {}

	virtual FString GetPromptText(ARSPlayerState* PlayerState = nullptr) const { return ""; }

	virtual ERSPurchaseRepeatPolicy GetRepeatPolicy() const { return ERSPurchaseRepeatPolicy::Once; }
};
