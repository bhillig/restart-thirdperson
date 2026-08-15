// Brandon Hillig 2026


#include "Purchasing/Effects/RSPurchaseEffect_GrantWeapon.h"

#include "ActorComponents/WeaponsComponent.h"
#include "Actors/RSWallBuy.h"
#include "PlayerStates/RSPlayerState.h"

void FRSPurchaseEffect_GrantWeapon::Apply(AActor* Owner, ARSPlayerState* Buyer) const
{
	ARSWallBuy* WallBuy = CastChecked<ARSWallBuy>(Owner);
	
	ensure(Buyer);
	IWeaponAimSource* WeaponAimSource = Cast<IWeaponAimSource>(Buyer->GetPawn());
	ensure(WeaponAimSource);

	if (WeaponAimSource->GetWeaponsComponent()->HasWeaponInInventory(WeaponData))
	{
		// Refill ammo
		WallBuy->RefillAmmo(WeaponData, Buyer);
		return;
	}

	// Grant weapon
	WallBuy->GrantWeapon(WeaponData, Buyer);
}

FString FRSPurchaseEffect_GrantWeapon::GetPromptText(ARSPlayerState* PlayerState) const
{
	ensureMsgf(WeaponData, TEXT("Weapon data isn't valid!"));
	ensure(PlayerState);

	IWeaponAimSource* WeaponAimSource = Cast<IWeaponAimSource>(PlayerState->GetPawn());
	ensure(WeaponAimSource);

	if (WeaponAimSource->GetWeaponsComponent()->HasWeaponInInventory(WeaponData))
	{
		return FString::Printf(TEXT("Refill Ammo for %s"), *WeaponData->Config.WeaponName.ToString());
	}
	return FString::Printf(TEXT("Buy %s"), *WeaponData->Config.WeaponName.ToString());
}

ERSPurchaseRepeatPolicy FRSPurchaseEffect_GrantWeapon::GetRepeatPolicy() const
{
	return ERSPurchaseRepeatPolicy::Unlimited;
}
