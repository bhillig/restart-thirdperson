// Brandon Hillig 2026


#include "Purchasing/Effects/RSPurchaseEffect_GrantWeapon.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void FRSPurchaseEffect_GrantWeapon::Apply(AActor* Owner, ARSPlayerState* Buyer)
{
	rs::LogOnce("Granted a weapon!");
}

ERSPurchaseRepeatPolicy FRSPurchaseEffect_GrantWeapon::GetRepeatPolicy()
{
	return ERSPurchaseRepeatPolicy::Unlimited;
}
