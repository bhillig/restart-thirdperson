// Brandon Hillig 2026


#include "ActorComponents/AttributesComponent.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

static TAutoConsoleVariable CVar_Attributes(TEXT("Debug.Attributes"), false, TEXT("Debug attributes component values"));

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	Health = MaxHealth;
}

void UAttributesComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UAttributesComponent::OnTakeAnyDamage);
	}
}

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttributesComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CVar_Attributes.GetValueOnGameThread())
	{
		const FString CurrentHealthMsg = FString::Printf(TEXT("%s [Health] = %f"), *GetOwner()->GetName(), Health);
		rs::LogTick(CurrentHealthMsg, -1, FColor::Green);
		const FString MaxHealthMsg = FString::Printf(TEXT("%s [Max Health] = %f"), *GetOwner()->GetName(), MaxHealth);
		rs::LogTick(MaxHealthMsg, -1, FColor::Emerald);
	}
}

bool UAttributesComponent::ApplyHealthDelta(float Delta, AController* EventInstigator, AActor* DamageCauser)
{
	const float OldHealth = Health;

	// Calculate the actual delta dealt
	const float NewHealth = FMath::Clamp(Health + Delta, 0.f, MaxHealth);
	const float ActualDelta = NewHealth - OldHealth;

	if (FMath::IsNearlyEqual(ActualDelta, 0))
	{
		return false;
	}

	Health = NewHealth;
	OnHealthChanged.Broadcast(NewHealth, MaxHealth, ActualDelta, EventInstigator, DamageCauser);

	if (FMath::IsNearlyEqual(Health, 0))
	{
		// A delta was applied and health now equals zero 
		OnDeath.Broadcast(EventInstigator, DamageCauser);
	}

	return true;
}

void UAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	ApplyHealthDelta(-Damage, InstigatedBy, DamageCauser);
}
