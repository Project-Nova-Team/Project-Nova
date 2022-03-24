#include "PatrolPath.h"

FPatrolPoint::FPatrolPoint(float Time, int32 Index, USplineComponent* Spline)
	: WaitTime(Time)
	, PointIndex(Index)
	, SplineComponent(Spline)
{}

APatrolPath::APatrolPath()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComponent);
}

FPatrolPoint APatrolPath::GetNextPoint(const int32 CurrentPointIndex, bool& bReversed, int32& OutPointIndex)
{
	if (Mode == Track)
	{
		int32 End = bReversed ? 0 : SplineComponent->GetNumberOfSplinePoints() - 1;
	
		if (CurrentPointIndex == End)
		{
			bReversed = !bReversed;
		}

		int32 Direction = bReversed ? -1 : 1;
		OutPointIndex = CurrentPointIndex + Direction;

		if (WaitTimes.Num() == 0)
		{
			WaitTimes.Add(0);
		}

		if (WaitTimes.Num() <= OutPointIndex)
		{
			WaitTimes.Add(0);
		}

		return FPatrolPoint(WaitTimes[OutPointIndex], OutPointIndex, SplineComponent);
	}

	else if (Mode == Loop)
	{
		OutPointIndex =
			CurrentPointIndex == SplineComponent->GetNumberOfSplinePoints() - 1
			? 0
			: CurrentPointIndex + 1;

		if (WaitTimes.Num() == 0)
		{
			WaitTimes.Add(0);
		}

		if (WaitTimes.Num() <= OutPointIndex)
		{
			WaitTimes.Add(0);
		}

		return FPatrolPoint(WaitTimes[OutPointIndex], OutPointIndex, SplineComponent);
	}

	return FPatrolPoint(-1, -1, nullptr);
}

FPatrolPoint APatrolPath::GetRandomPoint(TArray<int32>& RememberedPoints, int32 MemoryLimit)
{
	TArray<int32> Available;
	Available.Reserve(SplineComponent->GetNumberOfSplinePoints());

	for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		if (!RememberedPoints.Contains(i))
		{
			Available.Emplace(i);
		}
	}

	int32 RandomSelection = FMath::RandRange(0, Available.Num() - 1);

	//If our remembered points are full, forget the oldest point
	if (RememberedPoints.Num() == MemoryLimit && MemoryLimit != 0)
	{
		RememberedPoints.RemoveAt(0);
	}

	RandomSelection = Available[RandomSelection];
	RememberedPoints.Emplace(RandomSelection); //implicit copy

	if (WaitTimes.Num() <= RandomSelection)
	{
		WaitTimes.Add(0);
	}

	return FPatrolPoint(WaitTimes[RandomSelection], RandomSelection, SplineComponent);
}

void APatrolPath::SetPathMode(const EPathMode NewMode)
{
	Mode = NewMode;

#if WITH_EDITORONLY_DATA
	SetPathModeImpl();
#endif
}

#if WITH_EDITORONLY_DATA
void APatrolPath::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	const FName PropertyName = (Event.Property != NULL) ? Event.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(APatrolPath, Mode))
	{
		SetPathModeImpl();
	}
}

void APatrolPath::SetPathModeImpl()
{
	switch (Mode)
	{
		case Track:
			SplineComponent->SetClosedLoop(false);
			break;
		case Loop:
			SplineComponent->SetClosedLoop(true);
			break;
		case Random:
			SplineComponent->SetClosedLoop(true);
			break;
	}
}
#endif //WITH_EDITOR