// Brandon Hillig 2026


#include "Purchasing/Effects/RSPurchaseEffect_UnlockDoor.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void FRSPurchaseEffect_UnlockDoor::Apply(AActor* Owner, ARSPlayerState* Buyer) const
{
	rs::LogOnce("Unlocked a door!");
}

FString FRSPurchaseEffect_UnlockDoor::GetPromptText() const
{
	return "Unlock Door";
}

ERSPurchaseRepeatPolicy FRSPurchaseEffect_UnlockDoor::GetRepeatPolicy() const
{
	return ERSPurchaseRepeatPolicy::Once;
}
