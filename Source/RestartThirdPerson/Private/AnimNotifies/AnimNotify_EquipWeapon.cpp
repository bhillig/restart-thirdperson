// Brandon Hillig 2026


#include "AnimNotifies/AnimNotify_EquipWeapon.h"

#include "ActorComponents/WeaponsComponent.h"

void UAnimNotify_EquipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IWeaponAimSource* WeaponAimSource = Cast<IWeaponAimSource>(MeshComp->GetOwner()))
	{
		WeaponAimSource->GetWeaponsComponent()->AN_NotifyWeaponEquipped();
	}
}
