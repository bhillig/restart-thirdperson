// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "Effects/RSPurchaseEffect.h"
#include "RSPurchasableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Purchasable Component"))
class RESTARTTHIRDPERSON_API URSPurchasableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSPurchasableComponent();

protected:
	/** Purchase Effect */
	UPROPERTY(EditAnywhere, Category = "Config")
	TInstancedStruct<FRSPurchaseEffect> Effect;

	/** Cost */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int32 CreditsCost = 100;
		
};
