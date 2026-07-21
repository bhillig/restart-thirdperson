// Brandon Hillig 2026


#include "ALS/AnimNotifies/AnimNotify_FootOnGround.h"

#include "Kismet/GameplayStatics.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void UAnimNotify_FootOnGround::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const FName& FootBoneName = FootType == EFootType::Left ? LeftFootBoneName : RightFootBoneName;

	const FVector Start = MeshComp->GetBoneLocation(FootBoneName);
	const FVector End = Start + (FVector::DownVector * 1000);

	FCollisionShape Shape;
	Shape.SetSphere(10.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MeshComp->GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	// Calculate what surface we are landing on
	FHitResult OutHit;
	if (MeshComp->GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Ground, Shape, QueryParams))
	{
		const EPhysicalSurface HitSurface = UGameplayStatics::GetSurfaceType(OutHit);

		// Determine what sound to play based on surface
		USoundBase* FootSound = nullptr;
		switch (HitSurface)
		{
		case SurfaceType_Glass:
			FootSound = FootType == EFootType::Left ? LeftFootOnGlassSound : RightFootOnGlassSound;
			break;
		case SurfaceType_Plaster:
		default:
			FootSound = FootType == EFootType::Left ? LeftFootOnPlasterSound : RightFootOnPlasterSound;
			break;
		}

		// Play that sound
		UGameplayStatics::PlaySoundAtLocation(MeshComp, FootSound, OutHit.ImpactPoint);
	}
}
