// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "RSActionSystemComponent.generated.h"

class URSAttributeSet;
struct FGameplayTag;
class URSAction;
/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	URSActionSystemComponent();

	/** Called when initializing the component */
	virtual void InitializeComponent() override;

	/** Request to start the action with a given tag */
	void StartAction(FGameplayTag InActionTag);

	/** Request to stop the action with a given tag */
	void StopAction(FGameplayTag InActionTag);

	/** Active gameplay tags on this pawn */
	UPROPERTY(BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

protected:
	/** Default actions to grant upon initialization */
	UPROPERTY(EditAnywhere, Category="Actions")
	TArray<TSubclassOf<URSAction>> DefaultActions;

	/** Attribute Set Class to instantiate */
	UPROPERTY(EditAnywhere, NoClear, Category="Attributes")
	TSubclassOf<URSAttributeSet> AttributeSetClass;

protected:
	/** Current attributes */
	UPROPERTY(Transient)
	TObjectPtr<URSAttributeSet> Attributes;

	/** Array of actions */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URSAction>> Actions;
};
