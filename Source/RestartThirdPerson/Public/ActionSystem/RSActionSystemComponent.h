// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RSAttributeSet.h"
#include "UObject/Object.h"
#include "RSActionSystemComponent.generated.h"

struct FGameplayTag;
class URSAction;

UENUM(BlueprintType)
enum EAttributeChangeType : uint8
{
	Base,
	Modifier,
	BaseOverride,
	Invalid
};

// Native delegate
DECLARE_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, float, /* New Value */ float, /* Old Value */ AController*, /* EventInstigator */ AActor* /* ActorInstigator */);

// Dynamic delegate
DECLARE_DYNAMIC_DELEGATE_FourParams(FOnAttributeChangedDynamic, float, NewValue, float, OldValue, AController*, EventInstigator, AActor*, InstigatorActor);

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	/** Request to start the action with a given tag */
	void StartAction(FGameplayTag InActionTag);

	/** Request to stop the action with a given tag */
	void StopAction(FGameplayTag InActionTag);

	/** Applies an attribute change */
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyAttributeChange(FGameplayTag InAttributeTag, float Delta, EAttributeChangeType ChangeType, AController* EventInstigator = nullptr, AActor* InstigatorActor = nullptr);

	/** Returns the attribute value for a given attribute */
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetAttributeValue(FGameplayTag InAttributeTag) const;

	/** USED TO REGISTER A NATIVE LISTENER */
	/** Retrieves the attribute listener of a given attribute tag, creates one if it doesn't exist yet */
	FOnAttributeChanged& GetAttributeListener(FGameplayTag InAttributeTag);

	/** REGISTER A BLUEPRINT LISTENER */
	UFUNCTION(BlueprintCallable, DisplayName="Add Attribute Listener", Category="Attributes", meta = (Keywords="event,delegate"))
	void AddDynamicAttributeListener(FGameplayTag InAttributeTag, FOnAttributeChangedDynamic Event);

	/** REMOVE A BLUEPRINT LISTENER */
	UFUNCTION(BlueprintCallable, DisplayName = "Remove Attribute Listener", Category = "Attributes", meta = (Keywords = "event,delegate"))
	void RemoveDynamicAttributeListener(FOnAttributeChangedDynamic Event);

	/** Active gameplay tags on this pawn */
	UPROPERTY(BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

protected:
	/** Finds the attribute of a given tag, returns nullptr if it doesn't exist */
	FRSAttribute* FindAttributeByTag(FGameplayTag InAttributeTag) const;

	/** Finds the owning attribute set of a given attribute, returns nullptr if it doesn't exist */
	URSAttributeSet* FindOwningAttributeSet(FRSAttribute* Attribute) const;

protected:
	/** Default actions to grant upon initialization */
	UPROPERTY(EditAnywhere, Category="Actions")
	TArray<TSubclassOf<URSAction>> DefaultActions;

protected:
	/** Current attribute sets */
	UPROPERTY(EditAnywhere, Instanced, Category="Action System")
	TArray<TObjectPtr<URSAttributeSet>> AttributeSets;

	/** Cached Attributes */
	TMap<FGameplayTag, FRSAttribute*> CachedAttributes;

	/** Cached Attribute Sets */
	TMap<FRSAttribute*, URSAttributeSet*> CachedAttributeSets;

	/** Attribute Listeners */
	TMap<FGameplayTag, FOnAttributeChanged> AttributeListeners;

	/** Blueprint Attribute Listeners */
	TMap<FGameplayTag, TArray<FOnAttributeChangedDynamic>> BlueprintAttributeListeners;

	/** Array of actions */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URSAction>> Actions;

public:
	/** Constructor */
	URSActionSystemComponent();

	/** Called when initializing the component */
	virtual void InitializeComponent() override;
};
