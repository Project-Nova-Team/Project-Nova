#include "ObjectiveSystem.h"
#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#include "../Slate/ObjectiveWidget.h"
#include "../Player/Shooter.h"

FObjective::FObjective()
	: bUseWaypoint(true)
	, WaypointType(WAYPOINT_Static)
	, HeightOffset(100.f)
	, DeactivationDistance(100.f)
{}

AObjectiveSystem::AObjectiveSystem()
{
	ArrowSprite = CreateDefaultSubobject<UBillboardComponent>("Arrow");
	SetRootComponent(ArrowSprite);

	DistanceText = CreateDefaultSubobject<UTextRenderComponent>("Distance Display");
	DistanceText->SetupAttachment(ArrowSprite);
	DistanceText->SetRelativeLocation(FVector(0.f, 0.f, -30.f));

	PrimaryActorTick.bCanEverTick = true;
}

void AObjectiveSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWaypointActive)
	{
		const float Distance = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
		const int Display = Distance * 0.01f;

		DistanceText->SetText(FText::FromString(FString::FromInt(Display) + TEXT("M")));

		if (ActiveObjective.WaypointType == WAYPOINT_Actor)
		{
			SetActorLocation(ActiveObjective.TargetActor->GetActorLocation() + FVector(0.f, 0.f, ActiveObjective.HeightOffset));
		}

		else if (Distance < ActiveObjective.DeactivationDistance)
		{
			WaypointIndex++;

			if (WaypointIndex >= ActiveObjective.TargetLocations.Num())
			{
				ClearWaypoint();
			}

			else
			{
				SetActorLocation(ActiveObjective.TargetLocations[WaypointIndex] + FVector(0.f, 0.f, ActiveObjective.HeightOffset));
			}
		}
	}
}

void AObjectiveSystem::ClearWaypoint()
{
	bWaypointActive = false;
	ArrowSprite->SetVisibility(false);
	DistanceText->SetVisibility(false);
}

void AObjectiveSystem::SetNewObjective(const FObjective& NewObjective)
{
	ActiveObjective = NewObjective;
	Widget->UpdateObjectiveWidget(ActiveObjective.ObjectiveText);

	const bool bValidActor = NewObjective.TargetActor != nullptr && NewObjective.WaypointType == WAYPOINT_Actor;
	const bool bValidStatic = NewObjective.TargetLocations.Num() > 0 && NewObjective.WaypointType == WAYPOINT_Static;

	if (NewObjective.bUseWaypoint && (bValidActor || bValidStatic))
	{
		bWaypointActive = true;
		ArrowSprite->SetVisibility(true);
		DistanceText->SetVisibility(true);

		if (ActiveObjective.WaypointType == WAYPOINT_Actor)
		{
			SetActorLocation(ActiveObjective.TargetActor->GetActorLocation() + FVector(0.f, 0.f, ActiveObjective.HeightOffset));
		}

		else
		{
			WaypointIndex = 0;
			SetActorLocation(ActiveObjective.TargetLocations[0] + FVector(0.f, 0.f, ActiveObjective.HeightOffset));
		}
	}
}
