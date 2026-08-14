// Brandon Hillig 2026


#include "Purchasing/Effects/RSPurchaseEffect_UnlockDoor.h"

#include "Actors/RSPurchasableDoor.h"

void FRSPurchaseEffect_UnlockDoor::Apply(AActor* Owner, ARSPlayerState* Buyer) const
{
	ARSPurchasableDoor* PurchasableDoor = CastChecked<ARSPurchasableDoor>(Owner);
	PurchasableDoor->UnlockDoor();
}

FString FRSPurchaseEffect_UnlockDoor::GetPromptText() const
{
	return "Unlock Door";
}

ERSPurchaseRepeatPolicy FRSPurchaseEffect_UnlockDoor::GetRepeatPolicy() const
{
	return ERSPurchaseRepeatPolicy::Once;
}
