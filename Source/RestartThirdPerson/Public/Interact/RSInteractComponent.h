// Brandon Hillig 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RSInteractComponent.generated.h"

struct FRSInteractionPrompt;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRSOnFocusedChangedDelegate, const FRSInteractionPrompt&, Prompt);

class URSInteractableComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RESTARTTHIRDPERSON_API URSInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Constructor */
	URSInteractComponent();

	/** Register replicated variables */
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Attempts to interact. Called from player class */
	void TryInteract();

protected:
	/** Maximum range of interaction */
	UPROPERTY(EditAnywhere, Category = "Interact")
	float MaxInteractRange = 150.f;

	/** Interval in which the focused interactable is updated */
	UPROPERTY(EditAnywhere, Category = "Interact")
	float InteractInterval = 0.1f;

protected:
	/** Client -> Server RPC to interact */
	UFUNCTION(Server, Reliable)
	void Server_Interact(URSInteractableComponent* Interactable);

	/** Server -> Owner RPC to notify focus changed */
	UFUNCTION(Client, Unreliable)
	void Client_NotifyFocusChanged(const FRSInteractionPrompt& Prompt);

protected:
	/** Called when the game begins */
	virtual void BeginPlay() override;

	/** Runs every PrimaryComponentTick.TickInterval */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Called when its owning pawn changes controllers */
	UFUNCTION()
	void OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

public:
	/** On Focused Changed Delegate */
	UPROPERTY(BlueprintAssignable)
	FRSOnFocusedChangedDelegate OnFocusedChanged;

protected:
	/** Server -> All: Notify clients to play a montage */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayMontage(UAnimMontage* Montage);

protected:
	/** Anim montage to play when interacting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> InteractMontage;

protected:
	/** Checks if the owning pawn is locally controlled and enables tick if so. Called on BeginPlay and when pawn controller changes */
	void RefreshTickState();

	/** Determines the interactable to focus on depending on the registered interactables. If none fit, sets focus to nullptr */
	void RefreshFocus();

	/** Sets a new interactable to focus on, nullptr if none. Called every InteractInterval */
	void SetFocus(URSInteractableComponent* InteractableComp);

protected:
	/** The focused interactable to interact with. Can be nullptr */
	UPROPERTY(Replicated)
	TObjectPtr<URSInteractableComponent> FocusedInteractable = nullptr;
		
};
