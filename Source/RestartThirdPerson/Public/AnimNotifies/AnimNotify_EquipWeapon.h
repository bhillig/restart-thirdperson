// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EquipWeapon.generated.h"

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UAnimNotify_EquipWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
	/** Called when the anim notify triggers */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
