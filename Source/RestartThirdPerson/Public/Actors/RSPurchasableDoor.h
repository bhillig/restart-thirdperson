// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "RSPurchasableDoor.generated.h"

class URSPurchasableComponent;

UCLASS()
class RESTARTTHIRDPERSON_API ARSPurchasableDoor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<URSPurchasableComponent> Purchasable;

public:	
	/** Constructor */
	ARSPurchasableDoor();

	/** Registers replicated variables */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/** Called when the game begins */
	virtual void BeginPlay() override;

	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;
public:
	/** Unlocks the door */
	UFUNCTION(BlueprintCallable)
	void UnlockDoor();

protected:
	/** Client -> Server RPC to open a door */
	UFUNCTION(Server, Reliable)
	void Server_OpenDoor();

protected:
	/** Handles logic run everywhere when a door is opened. Called on server and clients */
	void HandleOpenedDoor();

protected:
	/** Timeline data */
	UPROPERTY()
	FTimeline OpenTimeline;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> OpenCurve;

	UFUNCTION()
	void HandleOpenProgress(float Value);

	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float OpenAngle = 90.f;

protected:
	/** State of whether the door is open */
	UPROPERTY(ReplicatedUsing=OnRep_IsOpen)
	bool bIsOpen = false;

	/** Rep notify for bIsOpen */
	UFUNCTION()
	void OnRep_IsOpen();

	/** Time on the server when the door was opened */
	UPROPERTY(Replicated)
	float ServerOpenTime;

};
