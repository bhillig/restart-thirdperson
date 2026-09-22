// Brandon Hillig 2026


#include "Pickup/RSCoinPickupSubsystem.h"

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

	// Load static mesh
	FSoftObjectPath MeshPath(TEXT("/Game/ThirdParty/ProjectOrionAssets/Content/ExampleContent/Meshes/SM_Pickup_Coin.SM_Pickup_Coin"));
	UStaticMesh* Mesh = Cast<UStaticMesh>(MeshPath.TryLoad());

	// Create instanced static mesh
	WorldISM = NewObject<UInstancedStaticMeshComponent>(&InWorld, NAME_None, RF_Transient);
	WorldISM->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WorldISM->SetStaticMesh(Mesh);
	WorldISM->RegisterComponentWithWorld(&InWorld);
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

	for (int32 i = 0; i < PlayerLocations.Num(); ++i)
	{
		for (int32 j = CoinLocations.Num() - 1; j >= 0; --j)
		{
			const float DistanceSqr = FVector::DistSquared(PlayerLocations[i], CoinLocations[j]);
			if (DistanceSqr < MaxDistanceSqr)
			{
				// Remove coin
				CoinLocations.RemoveAt(j);
				CoinPoints.RemoveAt(j);
				WorldISM->RemoveInstanceById(CoinInstanceIDs[j]);
				CoinInstanceIDs.RemoveAt(j);
			}
		}
	}
}

TStatId URSCoinPickupSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URSCoinPickupSubsystem, STATGROUP_Tickables);
}
