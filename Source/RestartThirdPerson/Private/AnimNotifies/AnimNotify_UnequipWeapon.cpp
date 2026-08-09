// Brandon Hillig 2026


#include "AnimNotifies/AnimNotify_UnequipWeapon.h"

#include "ActorComponents/WeaponsComponent.h"

void UAnimNotify_UnequipWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IWeaponAimSource* WeaponAimSource = Cast<IWeaponAimSource>(MeshComp->GetOwner()))
	{
		WeaponAimSource->GetWeaponsComponent()->AN_NotifyWeaponUnequipped();
	}
}
