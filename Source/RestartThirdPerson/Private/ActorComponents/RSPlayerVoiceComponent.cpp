// Brandon Hillig 2026


#include "ActorComponents/RSPlayerVoiceComponent.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RestartThirdPerson/RSGameplayTags.h"

URSPlayerVoiceComponent::URSPlayerVoiceComponent()
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
}

void URSPlayerVoiceComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Get action system component and bind delegates
	if (URSActionSystemComponent* ActionSystemComponent = GetOwner()->FindComponentByClass<URSActionSystemComponent>())
	{
		FOnAttributeChanged& HealthChangedEvent = ActionSystemComponent->GetAttributeListener(RSGameplayTags::Attribute_Health);
		HealthChangedEvent.AddUObject(this, &URSPlayerVoiceComponent::OnHealthChanged);
	}
}

void URSPlayerVoiceComponent::OnHealthChanged(float NewHealth, float OldHealth, AController* EventInstigator, AActor* DamageCauser)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Must be on the server
	if (FMath::IsNearlyZero(NewHealth))
	{
		OnDeath(EventInstigator, DamageCauser);
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
