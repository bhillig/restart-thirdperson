// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSInteractableComponent.generated.h"

class ARSPlayerState;

USTRUCT(BlueprintType)
struct FRSInteractionPrompt
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FText Text;

	UPROPERTY(BlueprintReadOnly)
	bool bEnabled = true;

	bool operator==(const FRSInteractionPrompt& Other) const
	{
		return Text.EqualTo(Other.Text);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API URSInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSInteractableComponent();

public:
	/** Client + server. Pure query - no side effects */
	virtual bool CanInteract(ARSPlayerState* PlayerState) const;

	/** Client. Called every focus refresh. Drives UI */
	virtual FRSInteractionPrompt GetInteractionPrompt(ARSPlayerState* PlayerState) const;

	/** Server only. Assumes CanInteract successfully passed */
	virtual void Interact(ARSPlayerState* PlayerState);

protected:
	/** Where the prompt draws relative to its location */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FVector PromptOffset = FVector(0, 0, 60);

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

	/** Called when the game ends */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
