// Brandon Hillig 2026


#include "Purchasing/Effects/RSPurchaseEffect_UnlockDoor.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void FRSPurchaseEffect_UnlockDoor::Apply(AActor* Owner, ARSPlayerState* Buyer)
{
	rs::LogOnce("Unlocked a door!");
}

ERSPurchaseRepeatPolicy FRSPurchaseEffect_UnlockDoor::GetRepeatPolicy()
{
	return ERSPurchaseRepeatPolicy::Once;
}
