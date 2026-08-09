// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_UnequipWeapon.generated.h"

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API UAnimNotify_UnequipWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	/** Called when the anim notify triggers */
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
