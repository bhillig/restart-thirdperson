// Brandon Hillig 2026


#include "ActorComponents/RSZombieVoiceComponent.h"

#include "Kismet/GameplayStatics.h"

URSZombieVoiceComponent::URSZombieVoiceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSZombieVoiceComponent::Enable()
{
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_ChaseSound, this, &URSZombieVoiceComponent::PlayChaseSound, ChaseInterval, true, 0.2f);
}

void URSZombieVoiceComponent::Disable()
{
	GetOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);
}

void URSZombieVoiceComponent::PlayChaseSound()
{
	UGameplayStatics::SpawnSoundAttached(ChaseZombieSound, this);
}
