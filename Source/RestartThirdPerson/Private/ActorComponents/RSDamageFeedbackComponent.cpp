// Brandon Hillig 2026


#include "ActorComponents/RSDamageFeedbackComponent.h"


URSDamageFeedbackComponent::URSDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void URSDamageFeedbackComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AActor* OwningActor = GetOwner())
	{
		OwningActor->OnTakePointDamage.AddDynamic(this, &URSDamageFeedbackComponent::OnTakePointDamage);
	}
}

void URSDamageFeedbackComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	// Get owning pawn
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn)
	{
		return;
	}

	// Get player controller
	APlayerController* PlayerController = Cast<APlayerController>(OwningPawn->GetController());
	if (!PlayerController)
	{
		return;
	}

	// Apply damage camera shake
	const FRotator ShakeSpace = ShotFromDirection.GetSafeNormal2D().Rotation();
	PlayerController->ClientStartCameraShake(DamageCameraShake, 1.f, ECameraShakePlaySpace::UserDefined, ShakeSpace);

	// Broadcast damage
	FRSDamageFeedbackEvent Event;
	Event.Damage = Damage;
	OnDamageFeedback.Broadcast(Event);
}
