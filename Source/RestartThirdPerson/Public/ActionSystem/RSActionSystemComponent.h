// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RSAttributeSet.h"
#include "UObject/Object.h"
#include "RSActionSystemComponent.generated.h"

struct FGameplayTag;
class URSAction;

UENUM()
enum EAttributeChangeType : uint8
{
	Base,
	Modifier,
	BaseOverride,
	Invalid
};

DECLARE_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, float, /* New Value */ float, /* Old Value */ AController*, /* EventInstigator */ AActor* /* ActorInstigator */);

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

	/** Applies an attribute change */
	bool ApplyAttributeChange(FGameplayTag InAttributeTag, float Delta, EAttributeChangeType ChangeType, AController* EventInstigator = nullptr, AActor* InstigatorActor = nullptr);

	/** Active gameplay tags on this pawn */
	UPROPERTY(BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer ActiveGameplayTags;

	/** Finds the attribute of a given tag, returns nullptr if it doesn't exist */
	FRSAttribute* FindAttributeByTag(FGameplayTag InAttributeTag);

	/** Retrieves the attribute delegate of a given attribute tag, creates one if it doesn't exist yet */
	FOnAttributeChanged& GetAttributeDelegate(FGameplayTag InAttributeTag);

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

	/** Cached Attributes */
	TMap<FGameplayTag, FRSAttribute*> CachedAttributes;

	/** Attribute Delegates */
	TMap<FGameplayTag, FOnAttributeChanged> AttributeDelegates;

	/** Array of actions */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URSAction>> Actions;
};
