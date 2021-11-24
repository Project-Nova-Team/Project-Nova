#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "../ShooterGameMode.h"
#include "../Utility/DelayedActionManager.h"
#include "Components/BillboardComponent.h"
#include "NavLinkRenderingComponent.h"
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

	NavLink = CreateDefaultSubobject<UNavLinkCustomComponent>("Nav Link");
	NavLink->SetNavigationRelevancy(true);
	NavLink->SetLinkData(FVector(-70, 0, 0), FVector(70, 0, 0), ENavLinkDirection::BothWays);

#if WITH_EDITORONLY_DATA
	EdRenderComp = CreateDefaultSubobject<UNavLinkRenderingComponent>(TEXT("EdRenderComp"));
	EdRenderComp->SetupAttachment(RootComponent);
#endif

#if WITH_EDITOR
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (!IsRunningCommandlet() && (SpriteComponent != NULL))
	{
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteTexture;
			FName ID_Decals;
			FText NAME_Decals;
			FConstructorStatics()
				: SpriteTexture(TEXT("/Engine/EditorResources/AI/S_NavLink"))
				, ID_Decals(TEXT("Navigation"))
				, NAME_Decals(NSLOCTEXT("SpriteCategory", "Navigation", "Navigation"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		SpriteComponent->Sprite = ConstructorStatics.SpriteTexture.Get();
		SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		SpriteComponent->bHiddenInGame = true;
		SpriteComponent->SetVisibleFlag(true);
		SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Decals;
		SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Decals;
		SpriteComponent->SetupAttachment(RootComponent);
		SpriteComponent->SetAbsolute(false, false, true);
		SpriteComponent->bIsScreenSizeScaled = true;
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 50));
	}
#endif

	DoorTransitionTime = 0.5f;
	DoorOpenDistance = 100.f;

	OnActorBeginOverlap.AddDynamic(this, &ADoor::ActorStartOverlap);
	OnActorEndOverlap.AddDynamic(this, &ADoor::ActorEndOverlap);
}

void ADoor::SetIsLocked(const bool Value)
{
	if (Value != bIsLocked)
	{
		SetIsLockedImpl(Value);
	}
}

void ADoor::SetIsLockedImpl(const bool Value)
{
	bIsLocked = Value;

	FEditorScriptExecutionGuard ScriptGuard; //lets us call this function during edit time;
	{
		ReceiveLockStatusChange(Value);
	}

	MaybeChangeDoorState();
	NavLink->SetEnabled(!Value);
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
		ReceiveDoorOpen();
		State = EDS_Changing;
		Handle = GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetDelayedActionManager()->StartOverTimeAction(
			this, &ADoor::OverTimeTransition, DoorTransitionTime, EDS_Open);
	}

	else if (ShouldClose())
	{
		ReceiveDoorClose();
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




#if WITH_EDITOR
void ADoor::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ADoor, bIsLocked))
	{
		SetIsLockedImpl(bIsLocked);
	}
}
#endif