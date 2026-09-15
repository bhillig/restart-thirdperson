// Brandon Hillig 2026


#include "ActionSystem/RSAction_Test.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction_Test::StartAction_Implementation()
{
	Super::StartAction_Implementation();

	rs::LogOnce("Test Action Started!", FColor::Purple, 3.0f);

	StopAction();
}
