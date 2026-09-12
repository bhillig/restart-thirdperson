// Brandon Hillig 2026


#include "ActionSystem/RSAction.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction::StartAction()
{
	rs::LogOnce(FString::Printf(TEXT("Starting action: %s"), *ActionName.ToString()), FColor::Green, 3.0f);
	UE_LOGFMT(LogTemp, Log, "Starting Action: {ActionName} at {WorldTime}", 
		("ActionName", ActionName), 
		("WorldTime", GetWorld()->GetTimeSeconds()));
}
