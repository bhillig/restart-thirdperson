// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RSGameplayLibrary.generated.h"

/** Game-specific utility functions usable from C++ and Blueprint */
UCLASS()
class RESTARTTHIRDPERSON_API URSGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns the number of active (non-spectating) players. Valid on server and clients via the replicated GameState PlayerArray. */
	UFUNCTION(BlueprintPure, Category = "RS|Gameplay", meta = (WorldContext = "WorldContextObject"))
	static int32 GetPlayerCount(const UObject* WorldContextObject);
};
