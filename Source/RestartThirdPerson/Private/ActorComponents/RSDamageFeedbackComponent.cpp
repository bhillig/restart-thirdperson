// Brandon Hillig 2026


#include "ActorComponents/RSDamageFeedbackComponent.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "RestartThirdPerson/RSGameplayTags.h"


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

		if (URSActionSystemComponent* ActionSystemComponent = OwningActor->FindComponentByClass<URSActionSystemComponent>())
		{
			FOnAttributeChanged& HealthChangedEvent = ActionSystemComponent->GetAttributeListener(RSGameplayTags::Attribute_Health);
			HealthChangedEvent.AddUObject(this, &URSDamageFeedbackComponent::OnHealthChanged);
		}
	}
}

void URSDamageFeedbackComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	ShotFromDirectionLast = ShotFromDirection;
	bUseDirection = true;
}

void URSDamageFeedbackComponent::OnHealthChanged(float NewHealth, float OldHealth, AController* EventInstigator, AActor* DamageCauser)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	const float Delta = NewHealth - OldHealth;

	// If we were healed
	if (Delta >= 0.f)
	{
		return;
	}

	Client_NotifyDamageTaken(-Delta);
}

void URSDamageFeedbackComponent::Client_NotifyDamageTaken_Implementation(float DamageTaken)
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
	Event.Damage = DamageTaken;
	OnDamageFeedback.Broadcast(Event);
}

