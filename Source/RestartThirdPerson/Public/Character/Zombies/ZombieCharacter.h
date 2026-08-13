// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

class URSZombieVoiceComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPawnDeathDelegate, AController*, InstigatedBy, bool, bWasHeadshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnHitDelegate, AController*, InstigatedBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPawnNoLongerStunnedDelegate);

class UAttributesComponent;

UCLASS()
class RESTARTTHIRDPERSON_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Attributes the zombie has */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UAttributesComponent> AttributesComponent;

	/** Voice component for the zombie */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URSZombieVoiceComponent> ZombieVoiceComponent;

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

	/** Config */
	UPROPERTY(EditAnywhere, Category = "Config")
	float AttackRange = 120.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float DamagePerHit = 30.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float CorpseLifeSpanAfterDeath = 60.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	FName HeadBoneName = "head";

public:
	/** Pawn Death Delegate */
	UPROPERTY(BlueprintAssignable)
	FPawnDeathDelegate OnPawnDeath;

	/** Pawn Hit Delegate */
	UPROPERTY(BlueprintAssignable)
	FPawnHitDelegate OnPawnHit;

	/** Pawn No Longer Stunned Delegate */
	UPROPERTY(BlueprintAssignable)
	FPawnNoLongerStunnedDelegate OnPawnNoLongerStunned;

protected:

	// TODO: Clean this up but just getting it to work for now

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HitReact();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Death();

protected:
	/** Actor the zombie is attacking, null when not attacking */
	UPROPERTY()
	TObjectPtr<AActor> VictimActor;

	/** State of whether the last shot applied was a headshot */
	UPROPERTY()
	bool bLastShotWasAHeadshot;

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
