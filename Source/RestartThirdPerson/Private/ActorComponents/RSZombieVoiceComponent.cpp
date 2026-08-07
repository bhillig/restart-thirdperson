// Brandon Hillig 2026


#include "ActorComponents/RSZombieVoiceComponent.h"

#include "ActorComponents/AttributesComponent.h"
#include "Kismet/GameplayStatics.h"

URSZombieVoiceComponent::URSZombieVoiceComponent()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
}

void URSZombieVoiceComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (UAttributesComponent* AttributesComponent = GetOwner()->FindComponentByClass<UAttributesComponent>())
	{
		// Listen for on death to stop the timer
		AttributesComponent->OnDeath.AddDynamic(this, &URSZombieVoiceComponent::OnDeath);
	}
}


void URSZombieVoiceComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_ChaseSound, this, &URSZombieVoiceComponent::PlayChaseSound, ChaseInterval, true);
}

void URSZombieVoiceComponent::OnDeath(AController* EventInstigator, AActor* DamageCauser)
{
	GetOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);
}

void URSZombieVoiceComponent::PlayChaseSound()
{
	UGameplayStatics::SpawnSoundAttached(ChaseZombieSound, this);
}
