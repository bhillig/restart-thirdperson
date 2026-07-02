// Brandon Hillig 2026


#include "ALS/AnimNotifies/AnimNotify_Land.h"

#include "Kismet/GameplayStatics.h"

void UAnimNotify_Land::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// Perform a raycast below the character and see what surface they are landing on
	const FVector Start = MeshComp->GetComponentLocation();
	const FVector End = Start + (FVector::DownVector * 1000);

	FCollisionShape Shape;
	Shape.SetSphere(30.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MeshComp->GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult OutHit;
	MeshComp->GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_Visibility, Shape, QueryParams);
	if (OutHit.bBlockingHit)
	{
		const EPhysicalSurface HitSurface = UGameplayStatics::GetSurfaceType(OutHit);

		USoundBase* LandSound = nullptr;
		switch (HitSurface)
		{
		case SurfaceType1: // Glass
			LandSound = GlassLandSound;
			break;
		case SurfaceType_Default: // Plaster
		default:
			LandSound = PlasterLandSound;
			break;
		}

		UGameplayStatics::PlaySoundAtLocation(MeshComp, LandSound, OutHit.ImpactPoint);
	}
}
