// Brandon Hillig 2026


#include "Actors/WeaponPickup.h"

#include "ActorComponents/WeaponsComponent.h"
#include "Components/SphereComponent.h"

AWeaponPickup::AWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>("TriggerSphere");
	TriggerSphere->SetupAttachment(WeaponMesh);
	TriggerSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

const FWeaponConfig& AWeaponPickup::GetWeaponConfig() const
{
	ensureMsgf(WeaponData, TEXT("WeaponData not set"));
	return WeaponData->Config;
}

void AWeaponPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	WeaponMesh->SetSkeletalMesh(WeaponData ? WeaponData->Config.Mesh : nullptr);
	WeaponMesh->SetOverlayMaterial(PickupOverlayMaterial);
}

void AWeaponPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickup::OnComponentBeginOverlap);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponPickup::OnComponentEndOverlap);
}

void AWeaponPickup::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UWeaponsComponent* WeaponsComp = FindWeaponsComponentOnActor(OtherActor))
	{
		WeaponsComp->AddWeaponPickupInRange(this);
	}
}

void AWeaponPickup::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (UWeaponsComponent* WeaponsComp = FindWeaponsComponentOnActor(OtherActor))
	{
		WeaponsComp->RemoveWeaponPickupInRange(this);
	}
}

UWeaponsComponent* AWeaponPickup::FindWeaponsComponentOnActor(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return nullptr;
	}

	TArray<UWeaponsComponent*> WeaponsComponents;
	OtherActor->GetComponents(UWeaponsComponent::StaticClass(), WeaponsComponents);

	return !WeaponsComponents.IsEmpty() ? WeaponsComponents[0] : nullptr;
}
