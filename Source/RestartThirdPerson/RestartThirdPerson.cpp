// Copyright Epic Games, Inc. All Rights Reserved.

#include "RestartThirdPerson.h"

#include "Kismet/KismetSystemLibrary.h"
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

void LogBool(const FString& VariableName, bool Value, FColor Color, float TimeToDisplay)
{
	if (GEngine)
	{
		const FString Message = FString::Printf(TEXT("%s = %s"), *VariableName, Value ? TEXT("True") : TEXT("False"));
		GEngine->AddOnScreenDebugMessage((int32)GetTypeHash(VariableName), TimeToDisplay, Color, Message);
	}
}

void LogFloat(const FString& VariableName, float Value, FColor Color, float TimeToDisplay)
{
	if (GEngine)
	{
		const FString Message = FString::Printf(TEXT("%s = %.2f"), *VariableName, Value);
		GEngine->AddOnScreenDebugMessage((int32)GetTypeHash(VariableName), TimeToDisplay, Color, Message);
	}
}

void DrawDebugArrowWithText(const FVector& Start, const FVector& End, const FString& Message, UWorld* World, FColor Color, float TimeToDisplay)
{
	UKismetSystemLibrary::DrawDebugArrow(World, Start, End, 32.f, Color, TimeToDisplay, 2.f);
	UKismetSystemLibrary::DrawDebugString(World, End, Message, nullptr, Color, TimeToDisplay);
}

}
