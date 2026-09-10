// Brandon Hillig 2026


#include "Interact/RSInteractComponent.h"

#include "GameFramework/Character.h"
#include "Interact/RSInteractableComponent.h"
#include "Interact/RSInteractionRegistry.h"
#include "PlayerStates/RSPlayerState.h"

static TAutoConsoleVariable CVarInteractDebugDraw(TEXT("Game.Interaction.DebugDraw"), false,
	TEXT("Enable interaction component debug drawing (0 = disabled, 1 = enabled)"));

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

	APawn* Pawn = CastChecked<APawn>(GetOwner());
	APlayerController* PC = CastChecked<APlayerController>(Pawn->GetController());

	const FVector PlayerCenter = Pawn->GetActorLocation();
	const FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();

	// Where the camera is looking
	const FVector CameraHitLocation = CameraLocation + PC->GetControlRotation().Vector() * MaxInteractRange;
	const FVector LookDirectionVector = (CameraHitLocation - CameraLocation).GetSafeNormal();

	const bool bDebugDraw = CVarInteractDebugDraw.GetValueOnGameThread();

	URSInteractableComponent* Best = nullptr;
	float HighestWeight = TNumericLimits<float>::Min();

	const float MaxInteractRangeSqr = MaxInteractRange * MaxInteractRange;

	for (URSInteractableComponent* Interactable : InteractRegistry->GetInteractables())
	{
		FVector InteractableOrigin;
		FVector InteractableExtents;
		Interactable->GetOwner()->GetActorBounds(false, InteractableOrigin, InteractableExtents);

		const float DistanceSqr = FVector::DistSquared(PlayerCenter, InteractableOrigin);

		// Normalized Distance [0-1] where 0 is maximum distance away and 1 is the closest it can be
		const float DistanceWeight = FMath::Clamp(1.f - (DistanceSqr / MaxInteractRangeSqr), 0.f, 1.f);

		const FVector CameraToInteractableDirectionVector = (InteractableOrigin - CameraLocation).GetSafeNormal();
		const float Dot = FVector::DotProduct(PC->GetControlRotation().Vector(), CameraToInteractableDirectionVector);

		// Normalized Direction [0-1] where 1 is looking directly at the object and 0 is looking away
		const float DirectionWeight = Dot * 0.5f + 0.5f;

		// Calculate total weight
		const float Weight = DistanceWeight * DistanceWeightScale + DirectionWeight * DirectionWeightScale;

#if !UE_BUILD_SHIPPING
		if (bDebugDraw)
		{
			const FString Msg = FString::Printf(TEXT("DistSqr: %f, Dot: %f, Total Weight: %f"), DistanceSqr, Dot, Weight);
			DrawDebugString(GetWorld(), InteractableOrigin, Msg, nullptr, FColor::White, InteractInterval, true);
		}
#endif

		if (DistanceSqr > MaxInteractRangeSqr || Dot < MinDotThreshold)
		{
			continue;
		}

		// Update best
		if (Weight > HighestWeight)
		{
			HighestWeight = Weight;
			Best = Interactable;
		}
	}
	
	SetFocus(Best);

#if !UE_BUILD_SHIPPING
	if (bDebugDraw)
	{
		for (URSInteractableComponent* Interactable : InteractRegistry->GetInteractables())
		{
			FVector InteractableOrigin;
			FVector InteractableExtents;
			Interactable->GetOwner()->GetActorBounds(false, InteractableOrigin, InteractableExtents);
			DrawDebugBox(GetWorld(), InteractableOrigin, InteractableExtents, Interactable == Best ? FColor::Green : FColor::White, false, InteractInterval);
		}
	}
#endif
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
