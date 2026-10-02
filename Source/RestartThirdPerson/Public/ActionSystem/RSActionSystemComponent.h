// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RSAttributeSet.h"
#include "UObject/Object.h"
#include "RSActionSystemComponent.generated.h"

class URSActionEffect;
struct FGameplayTag;
class URSAction;

UENUM(BlueprintType)
enum class EAttributeChangeType : uint8
{
	Base,
	Modifier,
	BaseOverride,
	Invalid
};

// Native delegate for changing an attribute
DECLARE_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, float, /* New Value */ float, /* Old Value */ AController*, /* EventInstigator */ AActor* /* ActorInstigator */);

// Dynamic delegate for changing an attribute
DECLARE_DYNAMIC_DELEGATE_FourParams(FOnAttributeChangedDynamic, float, NewValue, float, OldValue, AController*, EventInstigator, AActor*, InstigatorActor);

// Native delegate for adding a gameplay tag
DECLARE_MULTICAST_DELEGATE(FOnGameplayTagAdded);

// Native delegate for removing a gameplay tag
DECLARE_MULTICAST_DELEGATE(FOnGameplayTagRemoved);

/**
 * 
 */
UCLASS()
class RESTARTTHIRDPERSON_API URSActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Actions

	/** Request to start the action with a given tag */
	void StartAction(FGameplayTag InActionTag);

	/** Request to stop the action with a given tag */
	void StopAction(FGameplayTag InActionTag);

public:
	// Attributes

	/** Applies an attribute change */
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyAttributeChange(FGameplayTag InAttributeTag, float Delta, EAttributeChangeType ChangeType, AController* EventInstigator = nullptr, AActor* InstigatorActor = nullptr);

	/** Returns the attribute value for a given attribute */
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetAttributeValue(FGameplayTag InAttributeTag) const;

public:
	// Action Effects

	/** Applies an action effect */
	UFUNCTION(BlueprintCallable, Category="Effects")
	URSActionEffect* ApplyActionEffect(TSubclassOf<URSActionEffect> ActionEffectClass);

	/** Removes an action effect instance */
	UFUNCTION(BlueprintCallable, Category="Effects")
	void RemoveActionEffect(URSActionEffect* ActionEffect);
	
	/** Removes an action effect with a given tag */
	UFUNCTION(BlueprintCallable, Category="Effects")
	void RemoveActionEffectWithTag(FGameplayTag ActionEffectTag);

public:
	// Gameplay Tags

	/** Appends gameplay tags the owning actor */
	UFUNCTION(BlueprintCallable, Category="Tags")
	void AppendGameplayTags(const FGameplayTagContainer& TagContainer);

	/** Adds a gameplay tag to the owning actor */
	UFUNCTION(BlueprintCallable, Category="Tags")
	void AddGameplayTag(FGameplayTag Tag);

	/** Removes gameplay tags from the owning actor */
	UFUNCTION(BlueprintCallable, Category="Tags")
	void RemoveGameplayTags(const FGameplayTagContainer& TagContainer);

	/** Removes a gameplay tag from the owning actor */
	UFUNCTION(BlueprintCallable, Category="Tags")
	void RemoveGameplayTag(FGameplayTag Tag);

	/** Returns whether the owning actor has any gameplay tag from a given gameplay tag container */
	UFUNCTION(BlueprintCallable, Category="Tags")
	bool HasAnyGameplayTagFrom(const FGameplayTagContainer& TagContainer);

	/** Returns whether the owning actor has a given gameplay tag */
	UFUNCTION(BlueprintCallable, Category="Tags")
	bool HasGameplayTag(FGameplayTag Tag);

public:
	// Attribute Listeners (listens for attribute changes)

	/** USED TO REGISTER A NATIVE LISTENER */
	/** Retrieves the attribute listener of a given attribute tag, creates one if it doesn't exist yet */
	FOnAttributeChanged& GetAttributeListener(FGameplayTag InAttributeTag);

	/** REGISTER A BLUEPRINT LISTENER */
	UFUNCTION(BlueprintCallable, DisplayName="Add Attribute Listener", Category="Attributes", meta = (Keywords="event,delegate"))
	void AddDynamicAttributeListener(FGameplayTag InAttributeTag, FOnAttributeChangedDynamic Event);

	/** REMOVE A BLUEPRINT LISTENER */
	UFUNCTION(BlueprintCallable, DisplayName = "Remove Attribute Listener", Category = "Attributes", meta = (Keywords = "event,delegate"))
	void RemoveDynamicAttributeListener(FOnAttributeChangedDynamic Event);

public:
	// Gameplay Tag Listeners (listens for adding/removal of gameplay tags)

	/** USED TO REGISTER A NATIVE LISTENER */
	FOnGameplayTagAdded& GetGameplayTagAddedListener(FGameplayTag Tag);

	/** USED TO REGISTER A NATIVE LISTENER */
	FOnGameplayTagRemoved& GetGameplayTagRemovedListener(FGameplayTag Tag);

protected:
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

	/** Native (C++) Gameplay Tag Added Listeners */
	TMap<FGameplayTag, FOnGameplayTagAdded> NativeGameplayTagsAddedListeners;

	/** Native (C++) Gameplay Tag Removed Listeners */
	TMap<FGameplayTag, FOnGameplayTagRemoved> NativeGameplayTagsRemovedListeners;

	/** Array of actions */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URSAction>> Actions;

	/** Array of action effects */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URSActionEffect>> ActionEffects;

public:
	/** Constructor */
	URSActionSystemComponent();

	/** Called when initializing the component */
	virtual void InitializeComponent() override;
};
