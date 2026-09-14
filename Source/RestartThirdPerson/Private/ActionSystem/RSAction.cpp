// Brandon Hillig 2026


#include "ActionSystem/RSAction.h"

#include "ActionSystem/RSActionSystemComponent.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction::StartAction_Implementation()
{
	rs::LogOnce(FString::Printf(TEXT("Starting action: %s"), *ActionName.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Starting Action: {ActionName} at {WorldTime}",
		("ActionName", ActionName),
		("WorldTime", GetWorld()->GetTimeSeconds()));
}

void URSAction::StopAction_Implementation()
{
	rs::LogOnce(FString::Printf(TEXT("Stopping action: %s"), *ActionName.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Stopping Action: {ActionName} at {WorldTime}",
		("ActionName", ActionName),
		("WorldTime", GetWorld()->GetTimeSeconds()));
}

URSActionSystemComponent* URSAction::GetOwningComponent() const
{
	return Cast<URSActionSystemComponent>(GetOuter());
}
