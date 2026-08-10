// Brandon Hillig 2026


#include "ActorComponents/RSPlayerVoiceComponent.h"

#include "ActorComponents/AttributesComponent.h"
#include "Kismet/GameplayStatics.h"

URSPlayerVoiceComponent::URSPlayerVoiceComponent()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
}

void URSPlayerVoiceComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Get attributes component and bind delegates
	if (UAttributesComponent* AttributesComponent = GetOwner()->GetComponentByClass<UAttributesComponent>())
	{
		AttributesComponent->OnHealthChanged.AddDynamic(this, &URSPlayerVoiceComponent::OnHealthChanged);
		AttributesComponent->OnDeath.AddDynamic(this, &URSPlayerVoiceComponent::OnDeath);
	}
}

void URSPlayerVoiceComponent::OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser)
{
	if (FMath::IsNearlyZero(NewHealth))
	{
		// Let OnDeath handle this state
		return;
	}

	if (HitReactSound)
	{
		UGameplayStatics::SpawnSoundAttached(HitReactSound, this);
	}
}

void URSPlayerVoiceComponent::OnDeath(AController* EventInstigator, AActor* DamageCauser)
{
	if (DeathSound)
	{
		UGameplayStatics::SpawnSoundAttached(DeathSound, this);
	}
}
