// Brandon Hillig 2026


#include "ActionSystem/RSAction.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction::StartAction_Implementation()
{
	bIsRunning = true;

	rs::LogOnce(FString::Printf(TEXT("Starting action: %s"), *ActionTag.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Starting Action: {ActionTag} at {WorldTime}",
		("ActionTag", ActionTag.ToString()),
		("WorldTime", GetWorld()->GetTimeSeconds()));
}

void URSAction::StopAction_Implementation()
{
	bIsRunning = false;

	rs::LogOnce(FString::Printf(TEXT("Stopping action: %s"), *ActionTag.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Stopping Action: {ActionTag} at {WorldTime}",
		("ActionTag", ActionTag.ToString()),
		("WorldTime", GetWorld()->GetTimeSeconds()));
}

bool URSAction::CanStart() const
{
	if (IsRunning())
	{
		rs::LogOnce("Can't start action. It's already running!", FColor::Red, 3.0f);
		return false;
	}

	if (GetTimeUntilCooldownExpires() > 0.f)
	{
		rs::LogOnce("Can't start action. Cooldown isn't over!", FColor::Red, 3.0f);
		return false;
	}

	URSActionSystemComponent* ActionSystemComponent = GetOwningComponent();
	ensure(ActionSystemComponent);

	if (ActionSystemComponent->ActiveGameplayTags.HasAny(BlockedTags))
	{
		rs::LogOnce("Can't start action. Instigator has blocked tags!", FColor::Red, 3.0f);
		return false;
	}

	return true;
}

float URSAction::GetTimeUntilCooldownExpires() const
{
	return FMath::Max(0.f, GameTimeActionBecomesActive - GetWorld()->GetTimeSeconds());
}

URSActionSystemComponent* URSAction::GetOwningComponent() const
{
	return Cast<URSActionSystemComponent>(GetOuter());
}
