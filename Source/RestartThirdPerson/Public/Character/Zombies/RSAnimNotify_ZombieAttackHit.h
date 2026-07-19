// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RSAnimNotify_ZombieAttackHit.generated.h"

/**
 *  Runs a zombie's attack hit check. Place on the contact frame of an attack
 *  montage. The swing only connects if the target is in range at that instant.
 *  Expects the mesh's owner to be an AZombieCharacter.
 */
UCLASS(meta=(DisplayName = "Zombie Attack Hit", Category = "Zombies"))
class RESTARTTHIRDPERSON_API URSAnimNotify_ZombieAttackHit : public UAnimNotify
{
	GENERATED_BODY()

	/** UAnimNotify Interface */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
