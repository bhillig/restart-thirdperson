// Brandon Hillig 2026


#include "Interact/RSInteractComponent.h"

#include "GameFramework/Character.h"
#include "Interact/RSInteractableComponent.h"
#include "Interact/RSInteractionRegistry.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStates/RSPlayerState.h"

URSInteractComponent::URSInteractComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URSInteractComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void URSInteractComponent::TryInteract()
{
	// Local early out
	if (!FocusedInteractable)
	{
		return;
	}

	Server_Interact(FocusedInteractable);
}


void URSInteractComponent::Server_Interact_Implementation(URSInteractableComponent* Interactable)
{
	AController* OwningController = GetOwner()->GetInstigatorController();
	ensure(OwningController);

	ARSPlayerState* PlayerState = OwningController->GetPlayerState<ARSPlayerState>();
	ensure(PlayerState);

	if (Interactable && Interactable->CanInteract(PlayerState))
	{
		Interactable->Interact(PlayerState);
		Multicast_PlayMontage(InteractMontage);
	}
}

void URSInteractComponent::Client_NotifyFocusChanged_Implementation(const FRSInteractionPrompt& Prompt)
{
	OnFocusedChanged.Broadcast(Prompt);
}

void URSInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.TickInterval = InteractInterval;

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		// Subscribe to pawn delegates
		OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic(this, &URSInteractComponent::OnPawnControllerChanged);
	}

	// Determine if we should enable tick
	RefreshTickState();
}

void URSInteractComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshFocus();
}

void URSInteractComponent::RefreshFocus()
{
	URSInteractionRegistry* InteractRegistry = GetWorld()->GetSubsystem<URSInteractionRegistry>();
	ensure(InteractRegistry);

	URSInteractableComponent* Best = nullptr;
	float ClosestDistance = 9999.f;

	for (URSInteractableComponent* Interactable : InteractRegistry->GetInteractables())
	{
		// Get distance between it and the player
		const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Interactable->GetOwner()->GetActorLocation());
		if (Distance > MaxInteractRange)
		{
			continue;
		}

		// TODO: Check if player is viewing in the general direction of the interactable

		// TODO: Check for line of sight

		// Update best
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			Best = Interactable;
		}
	}

	SetFocus(Best);
}

void URSInteractComponent::SetFocus(URSInteractableComponent* InteractableComp)
{
	// Set new focus
	FocusedInteractable = InteractableComp;

	AController* OwningController = GetOwner()->GetInstigatorController();
	ensure(OwningController);

	ARSPlayerState* PlayerState = OwningController->GetPlayerState<ARSPlayerState>();
	if (!PlayerState) return;

	// Show new prompt
	if (FocusedInteractable && FocusedInteractable->CanInteract(PlayerState))
	{
		OnFocusedChanged.Broadcast(FocusedInteractable->GetInteractionPrompt(PlayerState));
	}
	else
	{
		OnFocusedChanged.Broadcast(FRSInteractionPrompt{});
	}
}

void URSInteractComponent::OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	RefreshTickState();
}

void URSInteractComponent::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
	ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
	if (!OwningCharacter)
	{
		return;
	}

	UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->Montage_Play(InteractMontage);
}

void URSInteractComponent::RefreshTickState()
{
	// Only tick if we are locally controlled
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	const bool bLocal = OwningPawn && OwningPawn->IsLocallyControlled();
	SetComponentTickEnabled(bLocal);
}
