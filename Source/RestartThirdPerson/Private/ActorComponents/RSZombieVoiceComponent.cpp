// Brandon Hillig 2026


#include "ActorComponents/RSZombieVoiceComponent.h"

#include "Kismet/GameplayStatics.h"

URSZombieVoiceComponent::URSZombieVoiceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void URSZombieVoiceComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_ChaseSound, this, &URSZombieVoiceComponent::PlayChaseSound, ChaseInterval, true);
}

void URSZombieVoiceComponent::PlayChaseSound()
{
	UGameplayStatics::SpawnSoundAttached(ChaseZombieSound, this);
}
