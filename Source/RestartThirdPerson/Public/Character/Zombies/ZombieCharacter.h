// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

class UAttributesComponent;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_StateTreeEvent_Zombie_StartChasing);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_StateTreeEvent_Zombie_StopChasing);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFinished, bool, bInterrupted);

UCLASS()
class RESTARTTHIRDPERSON_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UAttributesComponent> AttributesComponent;

public:
	AZombieCharacter();

	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnAttackFinished OnAttackFinished;

	// Attack Player (called via BTTask_CloseRangedAttack)
	bool Attack(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Death")
	bool IsDead();

protected:
	virtual void PostInitializeComponents() override;

protected:
	// Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> FireReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HeadReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	// Params
	UPROPERTY(EditAnywhere, Category = "Death")
	float CorpseLifeSpanAfterDeath = 60.f;

protected:

	UFUNCTION()
	void OnZombieTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void OnZombieDeath(AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	FTimerHandle FireReactTimerHandle; // Timer handle for callback upon hit fire react concluding

	bool bIsDead = false;
};
