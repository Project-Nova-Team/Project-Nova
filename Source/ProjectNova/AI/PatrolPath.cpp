#include "PatrolPath.h"

FPatrolPoint::FPatrolPoint(float Time, FSplinePoint Point)
	: WaitTime(Time)
	, SplinePoint(Point)
{}

APatrolPath::APatrolPath()
{
	SplineComponent = CreateEditorOnlyDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent->RemoveSplinePoint(1);
	SplineComponent->RemoveSplinePoint(0);

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

		
		return FPatrolPoint(WaitTimes[OutPointIndex], 
			FSplinePoint(OutPointIndex, SplineComponent->GetLocationAtSplineInputKey(OutPointIndex, ESplineCoordinateSpace::World), ESplinePointType::Linear));
	}

	else if (Mode == Loop)
	{
		OutPointIndex =
			CurrentPointIndex == SplineComponent->GetNumberOfSplinePoints() - 1
			? 0
			: CurrentPointIndex + 1;

		return FPatrolPoint(WaitTimes[OutPointIndex], 
			FSplinePoint(OutPointIndex, SplineComponent->GetLocationAtSplineInputKey(OutPointIndex, ESplineCoordinateSpace::World), ESplinePointType::Linear));
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error! GetNextPointLocation can only be called when a path is in Track or Loop Mode"));
		return FPatrolPoint(0.f, FSplinePoint());
	}
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

	return FPatrolPoint(WaitTimes[RandomSelection], 
		FSplinePoint(RandomSelection, SplineComponent->GetLocationAtSplineInputKey(RandomSelection, ESplineCoordinateSpace::World), ESplinePointType::Linear));
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