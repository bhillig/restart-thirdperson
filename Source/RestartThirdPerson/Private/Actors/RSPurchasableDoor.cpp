// Brandon Hillig 2026


#include "Actors/RSPurchasableDoor.h"

#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Purchasing/RSPurchasableComponent.h"

ARSPurchasableDoor::ARSPurchasableDoor()
{
	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetupAttachment(GetRootComponent());

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraint");
	PhysicsConstraint->SetupAttachment(GetRootComponent());
	PhysicsConstraint->SetConstrainedComponents(DoorMesh, NAME_None, nullptr, NAME_None);

	//PhysicsConstraint->SetAngularSwing1Limit(ACM_Locked, 85.f);
	//PhysicsConstraint->SetAngularSwing2Limit(ACM_Locked, 45.f);
	//PhysicsConstraint->SetAngularTwistLimit(ACM_Locked, 45.f);

	Purchasable = CreateDefaultSubobject<URSPurchasableComponent>("Purchasable");
}

void ARSPurchasableDoor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsOpen);
	DOREPLIFETIME(ThisClass, ServerOpenTime);
}

void ARSPurchasableDoor::BeginPlay()
{
	Super::BeginPlay();

	if (OpenCurve)
	{
		FOnTimelineFloat ProgressDelegate;
		ProgressDelegate.BindUFunction(this, FName("HandleOpenProgress"));
		OpenTimeline.AddInterpFloat(OpenCurve, ProgressDelegate);
	}
}

void ARSPurchasableDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	OpenTimeline.TickTimeline(DeltaSeconds);
}

void ARSPurchasableDoor::UnlockDoor()
{
	Server_OpenDoor();
}

void ARSPurchasableDoor::HandleOpenedDoor()
{
	const float Elapsed = GetWorld()->GetTimeSeconds() - ServerOpenTime;
	if (Elapsed > OpenTimeline.GetTimelineLength())
	{
		// Joined late / just became relevant. Snap to the end
		OpenTimeline.SetPlaybackPosition(OpenTimeline.GetTimelineLength(), true, true);
	}
	else
	{
		OpenTimeline.SetPlaybackPosition(FMath::Max(0.f, Elapsed), true, false);
		OpenTimeline.Play();
	}
}

void ARSPurchasableDoor::HandleOpenProgress(float Value)
{
	DoorMesh->SetRelativeRotation(FRotator(0.f, OpenAngle * Value, 0.f));
}

void ARSPurchasableDoor::Server_OpenDoor_Implementation()
{
	bIsOpen = true;
	ServerOpenTime = GetWorld()->GetTimeSeconds();

	HandleOpenedDoor();
}

void ARSPurchasableDoor::OnRep_IsOpen()
{
	if (!bIsOpen)
	{
		return;
	}

	HandleOpenedDoor();
}
