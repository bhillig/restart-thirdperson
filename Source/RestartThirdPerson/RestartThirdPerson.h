// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace rs
{

void LogOnce(const FString& Msg, FColor Color = FColor::White, float TimeToDisplay = 1.f);

void LogTick(const FString& Msg, int32 Key, FColor Color = FColor::White);

}
