// Copyright Epic Games, Inc. All Rights Reserved.

#include "RestartThirdPerson.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, RestartThirdPerson, "RestartThirdPerson" );

namespace rs
{

void LogMessage(const FString& Msg, FColor Color, float TimeToDisplay)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(FMath::Rand(), TimeToDisplay, Color, Msg);
	}
}

}
