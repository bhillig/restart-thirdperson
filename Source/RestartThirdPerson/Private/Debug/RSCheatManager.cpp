// Brandon Hillig 2026


#include "Debug/RSCheatManager.h"

#include "ActorComponents/WeaponsComponent.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void URSCheatManager::GiveWeapon(const FString& WeaponName)
{
	UWeaponsComponent* WeaponsComponent = GetWeaponsComponent();
	ensure(WeaponsComponent);

	TArray<const UWeaponDataAsset*> Weapons;
	GatherWeaponDataAssets(Weapons);

	const UWeaponDataAsset* const* Match = Weapons.FindByPredicate([&WeaponName](const UWeaponDataAsset* Weapon)
		{
			return Weapon->GetName().Contains(WeaponName) || Weapon->Config.WeaponName.ToString().Contains(WeaponName);
		});

	if (!Match)
	{
		const FString Msg = FString::Printf(TEXT("GiveWeapon: no weapon matching %s"), *WeaponName);
		rs::LogOnce(Msg, FColor::Red, 10.0f);
		ListWeapons();
		return;
	}

	WeaponsComponent->TryAddWeapon(*Match);
}

void URSCheatManager::ListWeapons()
{
	TArray<const UWeaponDataAsset*> Weapons;
	GatherWeaponDataAssets(Weapons);

	for (const UWeaponDataAsset* Weapon : Weapons)
	{
		rs::LogOnce(Weapon->GetName(), FColor::Blue, 10.f);
	}
}

void URSCheatManager::GatherWeaponDataAssets(TArray<const UWeaponDataAsset*>& OutWeapons)
{
	FARFilter Filter;
	Filter.ClassPaths.Add(UWeaponDataAsset::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;

	TArray<FAssetData> FoundAssets;
	IAssetRegistry::GetChecked().GetAssets(Filter, FoundAssets);

	for (const FAssetData& Asset : FoundAssets)
	{
		if (const UWeaponDataAsset* WeaponDataAsset = Cast<UWeaponDataAsset>(Asset.GetAsset()))
		{
			OutWeapons.Add(WeaponDataAsset);
		}
	}
}

UWeaponsComponent* URSCheatManager::GetWeaponsComponent() const
{
	const APlayerController* PC = GetOuterAPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UWeaponsComponent>() : nullptr;
}
