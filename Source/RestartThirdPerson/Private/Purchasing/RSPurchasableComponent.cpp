// Brandon Hillig 2026


#include "Purchasing/RSPurchasableComponent.h"

#include "Net/UnrealNetwork.h"
#include "PlayerStates/RSPlayerState.h"

URSPurchasableComponent::URSPurchasableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSPurchasableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URSPurchasableComponent, bPurchased);
	DOREPLIFETIME(URSPurchasableComponent, Purchasers);
}

bool URSPurchasableComponent::CanInteract(ARSPlayerState* PlayerState) const
{
	// Get effect
	const FRSPurchaseEffect* PurchaseEffect = Effect.GetPtr<FRSPurchaseEffect>();
	ensureMsgf(PurchaseEffect, TEXT("Purchase effect is not set!"));

	// Handle one-time purchase effects (e.g turning on a generator)
	if (PurchaseEffect->GetRepeatPolicy() == ERSPurchaseRepeatPolicy::Once)
	{
		return !bPurchased;
	}

	// Handle once per player purchase effects (e.g buying a perk)
	if (PurchaseEffect->GetRepeatPolicy() == ERSPurchaseRepeatPolicy::OncePerPlayer)
	{
		return !Purchasers.Contains(PlayerState);
	}

	// Otherwise unlimited
	return true;
}

FRSInteractionPrompt URSPurchasableComponent::GetInteractionPrompt(ARSPlayerState* PlayerState) const
{
	// Get effect
	const FRSPurchaseEffect* PurchaseEffect = Effect.GetPtr<FRSPurchaseEffect>();
	ensureMsgf(PurchaseEffect, TEXT("Purchase effect is not set!"));

	FRSInteractionPrompt Prompt;
	const FString HoldString = FString::Printf(TEXT("Press E to "));
	const FString CreditsMsg = FString::Printf(TEXT(" for %d credits"), CreditsCost);
	Prompt.Text = FText::FromString(HoldString + PurchaseEffect->GetPromptText() + CreditsMsg);
	Prompt.bEnabled = PlayerState->GetAvailableCredits() >= CreditsCost;
	return Prompt;
}

void URSPurchasableComponent::Interact(ARSPlayerState* PlayerState)
{
	ensure(PlayerState);

	if (!PlayerState->TrySpendCredits(CreditsCost))
	{
		// If the player can't afford this purchasable
		return;
	}

	// Get effect
	const FRSPurchaseEffect* PurchaseEffect = Effect.GetPtr<FRSPurchaseEffect>();
	ensureMsgf(PurchaseEffect, TEXT("Purchase effect is not set!"));

	// Apply purchase effect
	PurchaseEffect->Apply(GetOwner(), PlayerState);

	// If this effect is one time only, mark it as purchased
	if (PurchaseEffect->GetRepeatPolicy() == ERSPurchaseRepeatPolicy::Once)
	{
		bPurchased = true;
	}

	// If this effect is once per player, mark it as purchased for the buyer
	else if (PurchaseEffect->GetRepeatPolicy() == ERSPurchaseRepeatPolicy::OncePerPlayer)
	{
		Purchasers.AddUnique(PlayerState);
	}

	// Otherwise unlimited
}

void URSPurchasableComponent::OnRep_Purchased()
{
}
