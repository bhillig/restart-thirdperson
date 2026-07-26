// Brandon Hillig 2026


#include "Purchasing/Effects/RSPurchaseEffect_GrantWeapon.h"

#include "ActorComponents/WeaponsComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void FRSPurchaseEffect_GrantWeapon::Apply(AActor* Owner, ARSPlayerState* Buyer) const
{
	rs::LogOnce("Granted a weapon!");
}

FString FRSPurchaseEffect_GrantWeapon::GetPromptText() const
{
	ensureMsgf(WeaponData, TEXT("Weapon data isn't valid!"));
	return FString::Printf(TEXT("Buy %s"), *WeaponData->Config.WeaponName.ToString());
}

ERSPurchaseRepeatPolicy FRSPurchaseEffect_GrantWeapon::GetRepeatPolicy() const
{
	return ERSPurchaseRepeatPolicy::Unlimited;
}
