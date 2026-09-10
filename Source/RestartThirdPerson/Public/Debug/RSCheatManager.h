// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "RSCheatManager.generated.h"

class UWeaponsComponent;
class UWeaponDataAsset;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSCheatManager : public UCheatManager
{
	GENERATED_BODY()
public:
	/** Gives the local player the weapon whose asset name matches WeaponName */
	UFUNCTION(Exec)
	void GiveWeapon(const FString& WeaponName);

	/** Logs every weapon data asset that GiveWeapon can find */
	UFUNCTION(Exec)
	void ListWeapons();

private:
	static void GatherWeaponDataAssets(TArray<const UWeaponDataAsset*>& OutWeapons);

	/** Gets the local player's weapon component */
	UWeaponsComponent* GetWeaponsComponent() const;
};
