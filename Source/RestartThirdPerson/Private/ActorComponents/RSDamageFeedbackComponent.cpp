// Brandon Hillig 2026


#include "ActorComponents/RSDamageFeedbackComponent.h"

#include "ActorComponents/AttributesComponent.h"


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
		if (UAttributesComponent* AttributesComponent = OwningActor->FindComponentByClass<UAttributesComponent>())
		{
			AttributesComponent->OnHealthChanged.AddDynamic(this, &URSDamageFeedbackComponent::OnHealthChanged);
		}
	}
}

void URSDamageFeedbackComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	ShotFromDirectionLast = ShotFromDirection;
	bUseDirection = true;
}

void URSDamageFeedbackComponent::OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser)
{
	// If we were healed
	if (Delta >= 0.f)
	{
		return;
	}

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

	if (bUseDirection)
	{
		// Apply directional damage camera shake
		const FRotator ShakeSpace = ShotFromDirectionLast.GetSafeNormal2D().Rotation();
		PlayerController->ClientStartCameraShake(DamageCameraShake, 1.f, ECameraShakePlaySpace::UserDefined, ShakeSpace);

		// Reset flag
		bUseDirection = false;
	}
	else
	{
		// Apply damage camera shake
		PlayerController->ClientStartCameraShake(DamageCameraShake);
	}


	// Broadcast damage
	FRSDamageFeedbackEvent Event;
	Event.Damage = -Delta;
	OnDamageFeedback.Broadcast(Event);
}

