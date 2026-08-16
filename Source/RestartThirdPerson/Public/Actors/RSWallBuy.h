// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSWallBuy.generated.h"

class UWeaponDataAsset;
class ARSPlayerState;
class URSPurchasableComponent;

UCLASS()
class RESTARTTHIRDPERSON_API ARSWallBuy : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URSPurchasableComponent> Purchasable;

public:	
	/** Constructor */
	ARSWallBuy();

	void GrantWeapon(const UWeaponDataAsset* WeaponData, ARSPlayerState* PlayerState);

	void RefillAmmo(const UWeaponDataAsset* WeaponData, ARSPlayerState* PlayerState);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	TObjectPtr<UMaterialInterface> MeshMaterial;

};
