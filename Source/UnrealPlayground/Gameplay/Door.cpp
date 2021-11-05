#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLinkCustomComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = false;

	Frame = CreateDefaultSubobject<UStaticMeshComponent>("Frame");
	SetRootComponent(Frame);

	LeftSide = CreateDefaultSubobject<UStaticMeshComponent>("Left Door");
	LeftSide->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform);

	RightSide = CreateDefaultSubobject<UStaticMeshComponent>("Right Door");
	RightSide->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform);

	Trigger = CreateDefaultSubobject<UBoxComponent>("Door Trigger");
	Trigger->AttachToComponent(Frame, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->SetCollisionProfileName("OverlapOnlyPawn");

	DoorTransitionTime = 0.5f;
	DoorOpenDistance = 100.f;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	//Intentionally avoiding a null check. We need blueprint to force this components existence
	NavLink = Cast<ANavLinkProxy>(FindComponentByClass<UChildActorComponent>()->GetChildActor());

	if (bStartLocked)
	{
		SetIsLocked(true);
		OnDoorLockStatusChange.Broadcast(true);
	}

	OnActorBeginOverlap.AddDynamic(this, &ADoor::ActorStartOverlap);
	OnActorEndOverlap.AddDynamic(this, &ADoor::ActorEndOverlap);
}

void ADoor::SetIsLocked(const bool Value)
{
	if (Value != bIsLocked)
	{
		bIsLocked = Value;
		OnDoorLockStatusChange.Broadcast(Value);
		MaybeChangeDoorState();
		NavLink->GetSmartLinkComp()->SetEnabled(!Value);

		//TODO test with better nav area class if getting caught happens 
		/*if (Value)
		{
			NavLink->GetSmartLinkComp()->ClearNavigationObstacle();
		}
		
		else
		{
			NavLink->GetSmartLinkComp()->AddNavigationObstacle(UNavArea::StaticClass(), FVector(100, 100, 100));
		}*/
	}
}

void ADoor::ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(APawn::StaticClass()))
	{
		CurrentPawnCount++;
		MaybeChangeDoorState();
	}
}

void ADoor::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(APawn::StaticClass()))
	{
		CurrentPawnCount--;
		MaybeChangeDoorState();
	}
}

void ADoor::MaybeChangeDoorState()
{
	if (ShouldOpen())
	{
		State = EDS_Changing;
		Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
			this, &ADoor::OverTimeTransition, DoorTransitionTime, EDS_Open);
	}

	else if (ShouldClose())
	{
		State = EDS_Changing;
		Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
			this, &ADoor::OverTimeTransition, DoorTransitionTime, EDS_Closed);
	}
}

void ADoor::OverTimeTransition(const EDoorState TargetState)
{
	//Doors are always side by side so the side we choose doesn't really matter since we'll just flip the axis for each side anyways
	const FVector Right = FVector::RightVector;
	float Offset;

	if (TargetState == EDS_Open)
	{		
		Offset = FMath::Lerp(0.f, DoorOpenDistance, Handle->CurrentActionProgress);
	}

	else
	{
		Offset = FMath::Lerp(DoorOpenDistance, 0.f, Handle->CurrentActionProgress);
	}

	LeftSide->SetRelativeLocation(Right * Offset);
	RightSide->SetRelativeLocation(-Right * Offset);

	//We've finished, transitioning. Decide if we should start closing or opening again because something might have occured during the transition
	if (Handle->CurrentActionProgress >= 1.f)
	{
		State = TargetState;
		MaybeChangeDoorState();
	}
}