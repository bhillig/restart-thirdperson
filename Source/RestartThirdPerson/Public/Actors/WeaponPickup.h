// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/WeaponsComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponPickup.generated.h"

class UWeaponsComponent;
class USphereComponent;

UCLASS()
class RESTARTTHIRDPERSON_API AWeaponPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponPickup();

	const FWeaponConfig& GetWeaponConfig() const;

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

protected:

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	// Weapon Config
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UWeaponDataAsset> WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	TObjectPtr<UMaterialInterface> PickupOverlayMaterial;

private:

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UWeaponsComponent* FindWeaponsComponentOnActor(AActor* OtherActor);

	TArray<TObjectPtr<UWeaponsComponent>> WeaponsComponentsInRange;
};
