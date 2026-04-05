// Copyright Epic Games, Inc. All Rights Reserved.

#include "RestartThirdPerson.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, RestartThirdPerson, "RestartThirdPerson" );

namespace rs
{

void LogOnce(const FString& Msg, FColor Color, float TimeToDisplay)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, Color, Msg);
	}
}

void LogTick(const FString& Msg, int32 Key, FColor Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(Key, 0.f, Color, Msg);
	}
}

}
