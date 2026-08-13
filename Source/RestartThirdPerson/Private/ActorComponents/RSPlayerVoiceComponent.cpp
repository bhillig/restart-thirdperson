// Brandon Hillig 2026


#include "ActorComponents/RSPlayerVoiceComponent.h"

#include "ActorComponents/AttributesComponent.h"
#include "Kismet/GameplayStatics.h"

URSPlayerVoiceComponent::URSPlayerVoiceComponent()
{
	SetIsReplicatedByDefault(true);
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
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	if (FMath::IsNearlyZero(NewHealth))
	{
		// Let OnDeath handle this state
		return;
	}

	if (HitReactSound)
	{
		Multicast_SpawnSoundAttached(HitReactSound);
	}
}

void URSPlayerVoiceComponent::OnDeath(AController* EventInstigator, AActor* DamageCauser)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (DeathSound)
	{
		Multicast_SpawnSoundAttached(DeathSound);
	}
}

void URSPlayerVoiceComponent::Multicast_SpawnSoundAttached_Implementation(USoundBase* Sound)
{
	UGameplayStatics::SpawnSoundAttached(Sound, this);
}
