#include "PatrolPoint.h"

#if WITH_EDITOR
	#include "Components/BillboardComponent.h"
	#include "Components/ArrowComponent.h" 
	#include "UObject/ConstructorHelpers.h"
#endif

APatrolPoint::APatrolPoint() 
	: bUseLookDirection(true)
	, WaitDuration(5.f)
{
#if WITH_EDITOR
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> SpriteTexture;
		FName ID_Decals;
		FName ID_Arrow;
		FText NAME_Decals;
		FText NAME_Arrow;
		FConstructorStatics()
			: SpriteTexture(TEXT("/Game/Art/Textures/DebugAI/T_DebugAI_MonsterPatrol_D"))
			, ID_Decals(TEXT("AI"))
			, ID_Arrow(TEXT("Arrow"))
			, NAME_Decals(NSLOCTEXT("SpriteCategory", "Patrol", "Patrol"))
			, NAME_Arrow(NSLOCTEXT("SpriteCategory", "Patrol", "Patrol"))
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
		SpriteComponent->SetRelativeScale3D(FVector(0.75f));
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 30.f));
		SpriteComponent->bIsScreenSizeScaled = true;
	}

	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	if (ArrowComponent != nullptr)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->ArrowSize = 2.f;
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Arrow;
		ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Arrow;
		ArrowComponent->SetupAttachment(RootComponent);
		ArrowComponent->SetRelativeLocation(FVector(0, 0, -50.f));
		ArrowComponent->bIsScreenSizeScaled = true;			
	}
#endif
}

void APatrolPoint::SetUseLookDirection(const bool bValue)
{
	if (bUseLookDirection != bValue)
	{
		bUseLookDirection = bValue;
		UpdateArrowVisibility();
	}
}

#if WITH_EDITOR

void APatrolPoint::UpdateArrowVisibility()
{
	bool bIsVisible = bUseLookDirection && OwningPath != nullptr;
	ArrowComponent->SetVisibility(bIsVisible);
}

void APatrolPoint::PostLoad()
{
	Super::PostLoad();

	if (OwningPath == nullptr || !bUseLookDirection)
	{
		ArrowComponent->SetVisibility(false);
	}
}

void APatrolPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(APatrolPoint, bUseLookDirection))
	{
		UpdateArrowVisibility();
	}
}
#endif