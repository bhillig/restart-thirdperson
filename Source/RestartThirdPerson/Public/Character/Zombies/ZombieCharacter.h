// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPawnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPawnStunnedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPawnNoLongerStunnedDelegate);

class UAttributesComponent;

UCLASS()
class RESTARTTHIRDPERSON_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Attributes the zombie has */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UAttributesComponent> AttributesComponent;

public:
	AZombieCharacter();

protected:
	virtual void PostInitializeComponents() override;

public:
	/** Attack actor (called via StateTree: ST_Zombie) */
	bool Attack(AActor* TargetActor);

	/** Perform hit check and apply damage if valid (called via AnimNotify_ZombieAttackHit) */
	bool PerformHitCheck();

protected:
	/** Animations */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> FireReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HeadReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	/** Params */
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 120.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamagePerHit = 30.f;

	UPROPERTY(EditAnywhere, Category = "Death")
	float CorpseLifeSpanAfterDeath = 60.f;

public:
	/** Pawn Death Delegate */
	UPROPERTY(BlueprintAssignable)
	FPawnDeathDelegate OnPawnDeath;

	/** Pawn Stunned Delegate */
	UPROPERTY(BlueprintAssignable)
	FPawnStunnedDelegate OnPawnStunned;

	/** Pawn No Longer Stunned Delegate */
	UPROPERTY(BlueprintAssignable)
	FPawnNoLongerStunnedDelegate OnPawnNoLongerStunned;

protected:
	/** Actor the zombie is attacking, null when not attacking */
	UPROPERTY()
	TObjectPtr<AActor> VictimActor;

protected:
	UFUNCTION()
	void OnZombieTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void OnFireReactMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnZombieDeath(AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
