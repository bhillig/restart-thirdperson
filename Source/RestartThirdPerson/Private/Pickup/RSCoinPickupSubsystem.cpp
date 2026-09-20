// Brandon Hillig 2026


#include "Pickup/RSCoinPickupSubsystem.h"

void URSCoinPickupSubsystem::AddCoinPickups(TArray<FVector> Locations, TArray<int32> Points)
{
	CoinLocations.Append(Locations);
	CoinPoints.Append(Points);
}

void URSCoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();

	for (int32 i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(World, CoinLocations[i], 8.f, FColor::White);
	}
}

TStatId URSCoinPickupSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URSCoinPickupSubsystem, STATGROUP_Tickables);
}
