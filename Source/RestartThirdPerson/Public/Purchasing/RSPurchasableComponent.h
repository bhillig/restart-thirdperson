// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "Effects/RSPurchaseEffect.h"
#include "Interact/RSInteractableComponent.h"
#include "RSPurchasableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Purchasable Component"))
class RESTARTTHIRDPERSON_API URSPurchasableComponent : public URSInteractableComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSPurchasableComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** URSInteractableComponent Begin */
	virtual bool CanInteract(ARSPlayerState* PlayerState) const override;

	virtual FRSInteractionPrompt GetInteractionPrompt(ARSPlayerState* PlayerState) const override;

	virtual void Interact(ARSPlayerState* PlayerState) override;
	/** URSInteractableComponent End */

protected:
	/** Purchase Effect */
	UPROPERTY(EditAnywhere, Category = "Config")
	TInstancedStruct<FRSPurchaseEffect> Effect;

	/** Cost */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	int32 CreditsCost = 100;

	/** ERSPurchasedRepeatedPolicy::Once */
	UPROPERTY(ReplicatedUsing=OnRep_Purchased)
	bool bPurchased = false;

	/** ERSPurchasedRepeatedPolicy::OncePerPlayer */
	UPROPERTY(Replicated)
	TArray<TObjectPtr<ARSPlayerState>> Purchasers;

protected:
	UFUNCTION()
	void OnRep_Purchased();
};
