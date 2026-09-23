// Brandon Hillig 2026


#include "Pickup/RSCoinPickupSubsystem.h"

#include "Components/AudioComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void GetAllPlayerLocations(const UWorld& World, TArray<FVector>& Locations)
{
	AGameStateBase* GameState = World.GetGameState();
	if (!GameState) return;

	for (const APlayerState* PS : GameState->PlayerArray)
	{
		if (!PS) continue;

		if (APawn* PlayerPawn = PS->GetPawn())
		{
			Locations.Add(PlayerPawn->GetActorLocation());
		}
	}
}

void URSCoinPickupSubsystem::AddCoinPickups(TArray<FVector> Locations, TArray<int32> Points)
{
	CoinLocations.Append(Locations);
	CoinPoints.Append(Points);

	TArray<FTransform> CoinTransforms;
	for (int32 i = 0; i < Locations.Num(); ++i)
	{
		CoinTransforms.Add(FTransform(CoinLocations[i] + FVector(0, 0, 50)));
	}

	TArray<FPrimitiveInstanceId> CoinInstanceIDsToAdd = WorldISM->AddInstancesById(CoinTransforms, true, false);
	CoinInstanceIDs.Append(CoinInstanceIDsToAdd);
}

void URSCoinPickupSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Create instanced static mesh
	WorldISM = NewObject<UInstancedStaticMeshComponent>(&InWorld, NAME_None, RF_Transient);
	WorldISM->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WorldISM->RegisterComponentWithWorld(&InWorld);

	// Create audio component
	WorldAudioComponent = NewObject<UAudioComponent>(&InWorld, NAME_None, RF_Transient);
	WorldAudioComponent->RegisterComponentWithWorld(&InWorld);

	const URSCoinPickupSubsystemSettings* CoinPickupSubsystemSettings = GetDefault<URSCoinPickupSubsystemSettings>();
	CoinPickedUpParameter = CoinPickupSubsystemSettings->CoinPickupTriggerParameter;

	// Kick off load static mesh
	CoinPickupSubsystemSettings->CoinMesh.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &ThisClass::OnCoinMeshLoaded));

	// Kick off load coin pickup sound
	CoinPickupSubsystemSettings->CoinPickupSound.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &ThisClass::OnAudioLoaded));
}

void URSCoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();

	for (int32 i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(World, CoinLocations[i], 8.f, FColor::White);
	}

	TArray<FVector> PlayerLocations;
	GetAllPlayerLocations(*World, PlayerLocations);

	constexpr float MaxDistanceSqr = 100 * 100;

	TArray<int32> CoinIndicesToRemove;

	for (int32 i = 0; i < PlayerLocations.Num(); ++i)
	{
		for (int32 j = CoinLocations.Num() - 1; j >= 0; --j)
		{
			const float DistanceSqr = FVector::DistSquared(PlayerLocations[i], CoinLocations[j]);
			if (DistanceSqr < MaxDistanceSqr)
			{
				CoinIndicesToRemove.Add(j);
			}
		}
	}

	// Remove coins
	for (int32 i = CoinIndicesToRemove.Num() - 1; i >= 0; --i)
	{
		const int32 IndexToRemove = CoinIndicesToRemove[i];
		CoinLocations.RemoveAt(IndexToRemove);
		CoinPoints.RemoveAt(IndexToRemove);
		WorldISM->RemoveInstanceById(CoinInstanceIDs[IndexToRemove]);
		CoinInstanceIDs.RemoveAt(IndexToRemove);
	}

	if (!CoinIndicesToRemove.IsEmpty())
	{
		// Play sound
		UpdatePickupSound();
	}
}

void URSCoinPickupSubsystem::UpdatePickupSound()
{
	if (!WorldAudioComponent->IsPlaying())
	{
		WorldAudioComponent->Play();
	}

	WorldAudioComponent->SetTriggerParameter(CoinPickedUpParameter);
}

void URSCoinPickupSubsystem::OnCoinMeshLoaded(const FSoftObjectPath& ObjectPath, UObject* Object)
{
	if (!Object)
	{
		// Failed to load
		return;
	}

	WorldISM->SetStaticMesh(Cast<UStaticMesh>(Object));
}

void URSCoinPickupSubsystem::OnAudioLoaded(const FSoftObjectPath& ObjectPath, UObject* Object)
{
	if (!Object)
	{
		// Failed to load
		return;
	}

	WorldAudioComponent->SetSound(Cast<USoundBase>(Object));
}

TStatId URSCoinPickupSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URSCoinPickupSubsystem, STATGROUP_Tickables);
}
