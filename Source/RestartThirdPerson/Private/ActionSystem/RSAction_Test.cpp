// Brandon Hillig 2026


#include "ActionSystem/RSAction_Test.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

void URSAction_Test::StartAction()
{
	Super::StartAction();

	rs::LogOnce("Test Action Started!", FColor::Purple, 3.0f);
}
