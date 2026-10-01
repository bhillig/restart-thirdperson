// Brandon Hillig 2026

#include "ActorComponents/RSHealthRegenComponent.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "RestartThirdPerson/RSGameplayTags.h"

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<bool> CVarEnableHealthRegen(TEXT("Game.EnableHealthRegen"), true,
	TEXT("Toggles health regeneration for all actors with a HealthRegenComponent"), ECVF_Cheat);
#endif

URSHealthRegenComponent::URSHealthRegenComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bWantsInitializeComponent = true;
}

void URSHealthRegenComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AActor* OwningActor = GetOwner())
	{
		ActionSystemComponent = OwningActor->FindComponentByClass<URSActionSystemComponent>();
		if (ensure(ActionSystemComponent))
		{
			FOnAttributeChanged& HealthChangedEvent = ActionSystemComponent->GetAttributeListener(RSGameplayTags::Attribute_Health);
			HealthChangedEvent.AddUObject(this, &ThisClass::OnHealthChanged);

			FOnAttributeChanged& HealthMaxChangedEvent = ActionSystemComponent->GetAttributeListener(RSGameplayTags::Attribute_HealthMax);
			HealthMaxChangedEvent.AddUObject(this, &ThisClass::OnHealthMaxChanged);
		}
	}
}

void URSHealthRegenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ActionSystemComponent)
	{
		return;
	}

	const float Health = ActionSystemComponent->GetAttributeValue(RSGameplayTags::Attribute_Health);
	const float HealthMax = ActionSystemComponent->GetAttributeValue(RSGameplayTags::Attribute_HealthMax);

	if (Health <= 0.f || Health >= HealthMax)
	{
		SetComponentTickEnabled(false);
		return;
	}

#if !UE_BUILD_SHIPPING
	if (!CVarEnableHealthRegen.GetValueOnGameThread())
	{
		return;
	}
#endif

	// Regenerate health towards max health
	const float HealthToAdd = FMath::Min(RegenRate * DeltaTime, HealthMax - Health);
	ActionSystemComponent->ApplyAttributeChange(RSGameplayTags::Attribute_Health, HealthToAdd, EAttributeChangeType::Base);
}

void URSHealthRegenComponent::OnHealthChanged(float NewHealth, float OldHealth, AController* EventInstigator, AActor* InstigatorActor)
{
	if (!GetOwner()->HasAuthority() || NewHealth >= OldHealth)
	{
		return;
	}

	// We were damaged on the server
	SetComponentTickEnabled(false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RegenDelay, this, &ThisClass::StartRegen, RegenDelay, false);
}

void URSHealthRegenComponent::OnHealthMaxChanged(float NewHealthMax, float OldHealthMax, AController* EventInstigator, AActor* InstigatorActor)
{
	if (!GetOwner()->HasAuthority() || NewHealthMax <= OldHealthMax)
	{
		return;
	}

	// We increased health max on the server
	StartRegen();
}

void URSHealthRegenComponent::StartRegen()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	SetComponentTickEnabled(true);
}
