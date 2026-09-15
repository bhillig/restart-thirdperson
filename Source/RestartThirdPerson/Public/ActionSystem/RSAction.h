// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "RSAction.generated.h"

class URSActionSystemComponent;
/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class RESTARTTHIRDPERSON_API URSAction : public UObject
{
	GENERATED_BODY()

public:
	/** Starts the action */
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction();

	/** Stops the action */
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StopAction();

	/** Returns the tag of the action */
	UFUNCTION(BlueprintPure, Category="Action")
	FGameplayTag GetActionTag() const
	{
		return ActionTag;
	}

	/** Returns whether this action can be started */
	bool CanStart() const;

	/** Returns whether the action is running */
	UFUNCTION(BlueprintPure, Category="Action")
	bool IsRunning() const
	{
		return bIsRunning;
	}

	/** Returns the seconds remaining before the cooldown expires */
	UFUNCTION(BlueprintPure, Category="Action")
	float GetTimeUntilCooldownExpires() const;

	/** Returns the owning action system component this ability is within */
	UFUNCTION(BlueprintPure, Category="Action")
	URSActionSystemComponent* GetOwningComponent() const;

protected:
	/** The tag of the action */
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FGameplayTag ActionTag;

	/** Cooldown Duration (s) between uses */ 
	UPROPERTY(EditDefaultsOnly, Category="Action")
	float CooldownDuration = 0.f;

	/** Tags granted upon the start of the action and removed at the end */
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FGameplayTagContainer GrantedTags;

	/** Tags that must not exist on the instigator in order to start the action */
	UPROPERTY(EditDefaultsOnly, Category="Action")
	FGameplayTagContainer BlockedTags;

protected:
	/** The state of whether this action is running */
	UPROPERTY(Transient)
	bool bIsRunning = false;

	/** Game time seconds when this action exits cooldown */
	UPROPERTY(Transient)
	float GameTimeActionBecomesActive = 0.f;
	
};
