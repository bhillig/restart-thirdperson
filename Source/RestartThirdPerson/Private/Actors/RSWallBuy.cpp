// Brandon Hillig 2026


#include "Actors/RSWallBuy.h"

#include "ActorComponents/WeaponsComponent.h"
#include "PlayerStates/RSPlayerState.h"
#include "Purchasing/RSPurchasableComponent.h"
#include "Purchasing/Effects/RSPurchaseEffect_GrantWeapon.h"
#include "Weapons/RSWeaponHelpers.h"

ARSWallBuy::ARSWallBuy()
{
	bNetLoadOnClient = true;
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetupAttachment(GetRootComponent());

	Purchasable = CreateDefaultSubobject<URSPurchasableComponent>("Purchasable");
}

void ARSWallBuy::GrantWeapon(const UWeaponDataAsset* WeaponData, ARSPlayerState* PlayerState)
{
	if (IWeaponAimSource* WeaponAimSource = Cast<IWeaponAimSource>(PlayerState->GetPawn()))
	{
		WeaponAimSource->GetWeaponsComponent()->TryAddWeapon(WeaponData);
	}
}

void ARSWallBuy::RefillAmmo(const UWeaponDataAsset* WeaponData, ARSPlayerState* PlayerState)
{
	if (IWeaponAimSource* WeaponAimSource = Cast<IWeaponAimSource>(PlayerState->GetPawn()))
	{
		WeaponAimSource->GetWeaponsComponent()->TryRefillAmmoForWeapon(WeaponData);
	}
}

void ARSWallBuy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	WeaponMesh->SetSkeletalMesh(nullptr);
	const FRSPurchaseEffect_GrantWeapon* GrantWeaponEffect = Purchasable->GetPurchaseEffect().GetPtr<FRSPurchaseEffect_GrantWeapon>();
	if (!GrantWeaponEffect)
	{
		return;
	}

	if (GrantWeaponEffect->WeaponData)
	{
		WeaponMesh->SetSkeletalMesh(GrantWeaponEffect->WeaponData->Config.Mesh);
	}
}
