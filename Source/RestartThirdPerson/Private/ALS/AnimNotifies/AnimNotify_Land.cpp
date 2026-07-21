// Brandon Hillig 2026


#include "ALS/AnimNotifies/AnimNotify_Land.h"

#include "Kismet/GameplayStatics.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void UAnimNotify_Land::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const FVector Start = MeshComp->GetComponentLocation();
	const FVector End = Start + (FVector::DownVector * 1000);

	FCollisionShape Shape;
	Shape.SetSphere(30.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MeshComp->GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	// Calculate what surface we are landing on
	FHitResult OutHit;
	if (MeshComp->GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Ground, Shape, QueryParams))
	{
		const EPhysicalSurface HitSurface = UGameplayStatics::GetSurfaceType(OutHit);

		// Determine what sound to play based on surface
		USoundBase* LandSound = nullptr;
		switch (HitSurface)
		{
		case SurfaceType_Glass:
			LandSound = GlassLandSound;
			break;
		case SurfaceType_Plaster:
		default:
			LandSound = PlasterLandSound;
			break;
		}

		// Play that sound
		UGameplayStatics::PlaySoundAtLocation(MeshComp, LandSound, OutHit.ImpactPoint);
	}
}
