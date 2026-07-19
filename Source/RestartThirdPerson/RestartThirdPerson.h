// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

/** Ground detection trace channel (DefaultEngine.ini: ECC_GameTraceChannel1).
 *  Use this for "where is the floor" queries. Helpful for ignoring other pawns during IK Traces as they block Visibility.
 */
constexpr ECollisionChannel ECC_Ground = ECC_GameTraceChannel1;

namespace rs
{

/** Logging functions */
void LogOnce(const FString& Msg, FColor Color = FColor::White, float TimeToDisplay = 1.f);

void LogTick(const FString& Msg, int32 Key, FColor Color = FColor::White);

template <typename TEnum>
void LogEnum(const FString& VariableName, TEnum Value, FColor Color = FColor::White, float TimeToDisplay = 0.f)
{
	UEnum* Enum = StaticEnum<TEnum>();
	const FString Message = FString::Printf(TEXT("%s = %s"), *VariableName, *Enum->GetNameStringByValue((int64)Value));
	GEngine->AddOnScreenDebugMessage((int32)GetTypeHash(VariableName), TimeToDisplay, Color, Message);
}
	
void LogBool(const FString& VariableName, bool Value, FColor Color = FColor::White, float TimeToDisplay = 0.f);

void LogInt(const FString& VariableName, int32 Value, FColor Color = FColor::White, float TimeToDisplay = 0.f);

void LogFloat(const FString& VariableName, float Value, FColor Color = FColor::White, float TimeToDisplay = 0.f);

void DrawDebugArrowWithText(const FVector& Start, const FVector& End, const FString& Message, UWorld* World, FColor Color = FColor::Red, float TimeToDisplay = 0.f);

}
