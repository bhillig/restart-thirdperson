// Brandon Hillig 2026


#include "ActionSystem/RSAction.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction::StartAction_Implementation()
{
	rs::LogOnce(FString::Printf(TEXT("Starting action: %s"), *ActionTag.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Starting Action: {ActionTag} at {WorldTime}",
		("ActionTag", ActionTag.ToString()),
		("WorldTime", GetWorld()->GetTimeSeconds()));
}

void URSAction::StopAction_Implementation()
{
	rs::LogOnce(FString::Printf(TEXT("Stopping action: %s"), *ActionTag.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Stopping Action: {ActionTag} at {WorldTime}",
		("ActionTag", ActionTag.ToString()),
		("WorldTime", GetWorld()->GetTimeSeconds()));
}

URSActionSystemComponent* URSAction::GetOwningComponent() const
{
	return Cast<URSActionSystemComponent>(GetOuter());
}
