// Brandon Hillig 2026


#include "Character/Zombies/RSAnimNotify_ZombieAttackHit.h"

#include "Character/Zombies/ZombieCharacter.h"

void URSAnimNotify_ZombieAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AZombieCharacter* ZombieCharacter = Cast<AZombieCharacter>(MeshComp->GetOwner()))
	{
		// Perform hit check
		ZombieCharacter->PerformHitCheck();
	}
}
