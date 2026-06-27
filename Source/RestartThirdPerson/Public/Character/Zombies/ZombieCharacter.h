// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

class UAttributesComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFinished, bool, bInterrupted);

UCLASS()
class RESTARTTHIRDPERSON_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AZombieCharacter();

	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnAttackFinished OnAttackFinished;

	// Attack Player (called via BTTask_CloseRangedAttack)
	bool Attack(AActor* TargetActor);

protected:
	virtual void PostInitializeComponents() override;

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAttributesComponent> AttributesComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> FireReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

protected:
	UFUNCTION()
	void OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void OnDeath(AController* EventInstigator, AActor* DamageCauser);

	FTimerHandle FireReactTimerHandle; // Timer handle for callback upon hit fire react concluding
	FTimerHandle DeathTimerHandle; // Timer handle for callback upon dying
};
