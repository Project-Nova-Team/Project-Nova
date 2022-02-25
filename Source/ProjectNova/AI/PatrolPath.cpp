#include "PatrolPath.h"
#include "PatrolPoint.h"

#if WITH_EDITOR
	#include "Engine/Engine.h"
	#include "LevelEditorViewport.h"
	#include "UObject/ConstructorHelpers.h"
	#include "Components/BillboardComponent.h"
	#include "Components/SplineComponent.h"
	#include "DrawDebugHelpers.h"
	
	constexpr float MAX_CAST_DISTANCE = 10000.f;
#endif

APatrolPath::APatrolPath()
{
#if WITH_EDITORONLY_DATA

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TArray<FSplinePoint> SplinePoints;

	for (int i = 0; i < SplinePoints.Num(); ++i)
	{
		APatrolPoint* Point = Points[i];
		SplinePoints.Add(FSplinePoint(i, Point->GetActorLocation(), ESplinePointType::Linear));
	}

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteTexture;
		FName ID_Decals;
		FName ID_Arrow;
		FText NAME_Decals;
		FText NAME_Arrow;
		FConstructorStatics()
			: SpriteTexture(TEXT("/Game/Art/Textures/DebugAI/T_DebugAI_Path_D"))
			, ID_Decals(TEXT("AI"))
			, NAME_Decals(NSLOCTEXT("SpriteCategory", "Patrol", "Patrol"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	SetRootComponent(SpriteComponent);

	if (!IsRunningCommandlet() && (SpriteComponent != nullptr))
	{
		SpriteComponent->Sprite = ConstructorStatics.SpriteTexture.Get();
		SpriteComponent->bHiddenInGame = true;
		SpriteComponent->SetVisibleFlag(true);
		SpriteComponent->SpriteInfo.Category = TEXT("AI");
		SpriteComponent->SpriteInfo.DisplayName = NSLOCTEXT("AI", "Debug", "Debug");
		SpriteComponent->SetAbsolute(false, false, true);
		SpriteComponent->SetRelativeScale3D(FVector(0.5f));
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 30.f));
		SpriteComponent->bIsScreenSizeScaled = true;
	}

	SplineComponent = CreateEditorOnlyDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent->RemoveSplinePoint(1);
	SplineComponent->RemoveSplinePoint(0);

	if (Points.Num() > 1)
	{			
		SplineComponent->AddPoints(SplinePoints);
	}
#endif //WITH_EDITOR
}

APatrolPoint* APatrolPath::GetNextPoint(const int32 CurrentPointIndex, bool& bReversed, int32& OutPointIndex)
{
	if (Mode == Track)
	{
		int32 End = bReversed ? 0 : Points.Num() - 1;
	
		if (CurrentPointIndex == End)
		{
			bReversed = !bReversed;
		}

		int32 Direction = bReversed ? -1 : 1;
		OutPointIndex = CurrentPointIndex + Direction;

		return Points[OutPointIndex];
	}

	else if (Mode == Loop)
	{
		OutPointIndex =
			CurrentPointIndex == Points.Num() - 1
			? 0
			: CurrentPointIndex + 1;

		return Points[OutPointIndex];
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error! GetNextPointLocation can only be called when a path is in Track or Loop Mode"));
		return nullptr;
	}
}

APatrolPoint* APatrolPath::GetRandomPoint(TArray<APatrolPoint*>& RememberedPoints, const int32 MemoryLimit)
{
	TArray<APatrolPoint*> Available;
	Available.Reserve(Points.Num());

	for (APatrolPoint* Point : Points)
	{
		if (!RememberedPoints.Contains(Point))
		{
			Available.Emplace(Point);
		}
	}

	int32 RandomSelection = FMath::RandRange(0, Available.Num() - 1); //upper bound inclusive ;(

	//If our remembered points are full, forget the oldest point
	if (RememberedPoints.Num() == MemoryLimit && MemoryLimit != 0)
	{
		RememberedPoints.RemoveAt(0);
	}

	RememberedPoints.Emplace(Available[RandomSelection]); //implicit copy

	return Available[RandomSelection];
}

void APatrolPath::SetPathMode(const EPathMode NewMode)
{
	Mode = NewMode;

#if WITH_EDITORONLY_DATA
	SetPathModeImpl();
#endif
}

#if WITH_EDITORONLY_DATA
void APatrolPath::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SplineComponent->GetNumberOfSplinePoints() > 1)
	{
		for (int32 i = 0; i < Points.Num(); ++i)
		{
			SplineComponent->SetLocationAtSplinePoint(i, Points[i]->GetActorLocation(), ESplineCoordinateSpace::World);
		}
	}

	if (IsSelected())
	{
		for (int32 i = 0; i < Points.Num(); ++i)
		{
			if (Points[i] != nullptr)
			{
				DrawDebugLine(GetWorld(), GetActorLocation(), Points[i]->GetActorLocation(), FColor::Red);
			}		
		}
	}
}

bool APatrolPath::ShouldTickIfViewportsOnly() const
{
	return GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor;
}

void APatrolPath::PostLoad()
{
	Super::PostLoad();

	Handle = GEditor->OnLevelActorDeleted().AddUObject(this, &APatrolPath::ReceiveEditorDeletedActor);
}

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
	//This gets hit we switched FROM random TO a mode that wants to use the spline
	if (Mode < Random && Points.Num() > 1 && SplineComponent->GetNumberOfSplinePoints() == 0)
	{
		for (int32 i = 0; i < Points.Num(); ++i)
		{
			SplineComponent->AddPoint(FSplinePoint(i, Points[i]->GetActorLocation(), ESplinePointType::Linear));
		}
	}

	switch (Mode)
	{
		case Track:
			SplineComponent->SetClosedLoop(false);
			break;
		case Loop:
			SplineComponent->SetClosedLoop(true);
			break;
		case Random:
			for (int32 i = SplineComponent->GetNumberOfSplinePoints(); i > 0; --i)
			{
				SplineComponent->RemoveSplinePoint(i-1);
			}
			break;
	}
}

FReply APatrolPath::AddPoint()
{
	//Get the editor viewport camera
	const FLevelEditorViewportClient* Client = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	
	if (Client == nullptr)
	{
		FReply::Handled(); //bad error handling
	}

	const FViewportCameraTransform& Transform = Client->GetViewTransform();

	const FVector Location = Transform.GetLocation();
	const FVector Forward = Transform.GetRotation().Quaternion().GetForwardVector();

	FHitResult Hit;

	//Trace from the camera to see where we should attempt to place the point
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Location, Forward * MAX_CAST_DISTANCE, ECC_WorldStatic);
	const FVector SpawnLocation = bHit ? Hit.ImpactPoint + Hit.ImpactNormal * 100.f : Location + (Forward * 100.f);

	bool bNeedsFirstSplinePoint = Points.Num() == 1;

	//Spawn the actor and add it to the points array
	APatrolPoint* Point = GetWorld()->SpawnActor<APatrolPoint>(SpawnLocation, FRotator::ZeroRotator);
	Points.Add(Point);
	Point->OwningPath = this;

	if (Points.Num() > 1)
	{
		if (bNeedsFirstSplinePoint)
		{
			SplineComponent->AddSplinePoint(Points[0]->GetActorLocation(), ESplineCoordinateSpace::World);
			SplineComponent->SetSplinePointType(0, ESplinePointType::Linear);
		}

		SplineComponent->AddSplinePoint(Point->GetActorLocation(), ESplineCoordinateSpace::World);
		SplineComponent->SetSplinePointType(Points.Num() - 1, ESplinePointType::Linear);
	}

	return FReply::Handled();
}

void APatrolPath::ReceiveEditorDeletedActor(AActor* Actor)
{
	if (Actor == this)
	{
		GEditor->OnLevelActorDeleted().Remove(Handle);

		for (APatrolPoint* Point : Points)
		{
			if (Point != nullptr)
			{
				Point->Destroy();
			}		
		}

		return;
	}

	APatrolPoint* AsPoint = Cast<APatrolPoint>(Actor);
	
	if (AsPoint != nullptr && Points.Contains(AsPoint))
	{
		int32 PointIndex = Points.IndexOfByKey(AsPoint);
		Points.Remove(AsPoint);

		SplineComponent->RemoveSplinePoint(PointIndex);

		if (Points.Num() == 1)
		{
			SplineComponent->RemoveSplinePoint(0);
		}
	}
}
#endif //WITH_EDITOR