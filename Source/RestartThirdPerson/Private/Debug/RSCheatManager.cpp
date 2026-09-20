// Brandon Hillig 2026


#include "Debug/RSCheatManager.h"

#include "NavigationSystem.h"
#include "ActionSystem/RSActionSystemComponent.h"
#include "ActorComponents/WeaponsComponent.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Pickup/RSCoinPickupSubsystem.h"
#include "RestartThirdPerson/RestartThirdPerson.h"
#include "RestartThirdPerson/RSGameplayTags.h"

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

void URSCheatManager::ApplyHealthChange(float Delta)
{
	URSActionSystemComponent* ActionSystemComponent = GetActionSystemComponent();
	ensure(ActionSystemComponent);

	ActionSystemComponent->ApplyAttributeChange(RSGameplayTags::Attribute_Health, Delta, Base, nullptr, nullptr);
}

void URSCheatManager::SpawnCoins(int32 Amount)
{
	APawn* PlayerPawn = GetOuterAPlayerController()->GetPawn();

	const FVector PlayerLocation = PlayerPawn->GetActorLocation();
	const FVector PlayerForwardVector = PlayerPawn->GetActorForwardVector();

	// Get the point 1050cm in front of the player
	const FVector CenterPoint = PlayerLocation + PlayerForwardVector * 1050;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(PlayerPawn);
	ensure(NavSystem);

	TArray<FVector> CoinLocations;
	TArray<int32> CoinPoints;
	for (int32 i = 0; i < Amount; ++i)
	{
		FNavLocation NavLocation;
		NavSystem->GetRandomPointInNavigableRadius(CenterPoint, 1024, NavLocation);

		CoinLocations.Add(NavLocation.Location);
		CoinPoints.Add(10);
	}

	// Get the coin subsystem
	URSCoinPickupSubsystem* CoinPickupSubsytem = GetWorld()->GetSubsystem<URSCoinPickupSubsystem>();
	ensure(CoinPickupSubsytem);

	// Add the coin pickups to the subsystem
	CoinPickupSubsytem->AddCoinPickups(CoinLocations, CoinPoints);
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

URSActionSystemComponent* URSCheatManager::GetActionSystemComponent() const
{
	const APlayerController* PC = GetOuterAPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<URSActionSystemComponent>() : nullptr;
}
