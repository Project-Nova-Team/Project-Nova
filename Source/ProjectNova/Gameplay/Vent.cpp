#include "Vent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "InteractableFusebox.h"
#include "../Player/Shooter.h"
#include "../State/State.h"
#include "../ShooterGameMode.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../State/FPS/Event/SVentState.h"
#include "../Utility/DelayedActionManager.h"
#include "../Animation/ShooterAnimInstance.h"
#include "DrawDebugHelpers.h"

AVent::AVent()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	SetRootComponent(SplineComponent);

	EntryFrame = CreateDefaultSubobject<UStaticMeshComponent>("Entry");
	EntryFrame->SetupAttachment(SplineComponent);

	EntryGrate = CreateDefaultSubobject<UStaticMeshComponent>("Entry Grate");
	EntryGrate->SetupAttachment(EntryFrame);

	ExitFrame = CreateDefaultSubobject<UStaticMeshComponent>("Exit");
	ExitFrame->SetupAttachment(SplineComponent);

	ExitGrate = CreateDefaultSubobject<UStaticMeshComponent>("Exit Grate");
	ExitGrate->SetupAttachment(ExitFrame);

	const FVector RelativeOffset(300.f, 0.f, 0.f);

	EntryFrame->SetRelativeLocation(RelativeOffset);
	ExitFrame->SetRelativeLocation(-RelativeOffset);

	SplineComponent->SetLocationAtSplinePoint(0, RelativeOffset, ESplineCoordinateSpace::Local);
	SplineComponent->SetLocationAtSplinePoint(1, -RelativeOffset, ESplineCoordinateSpace::Local);
	SplineComponent->Duration = 1.f;
}

void AVent::SetVentDisabled(const bool bDisableVent)
{
	bDisabled = bDisableVent;
	EntryGrate->SetVisibility(!bDisabled);
	ExitGrate->SetVisibility(!bDisabled);
}

void AVent::InteractionEvent(APawn* EventSender)
{
	if (AShooter* Shooter = Cast<AShooter>(EventSender))
	{
		IInteractiveObject::InteractionEvent(EventSender);

		bInUse = true;
		const FString StateName = TEXT("Venting");
		USVentState* VentState = Shooter->GetStateMachine()->GetStateAtKey<USVentState>(StateName);
		VentState->Vent = this;

		//Hacky way to determine which vent was entered
		const FVector ShooterLocation(Shooter->GetActorLocation());
		const bool bEnteredVentFromEntryGrate =
			(EntryGrate->GetComponentLocation() - Shooter->GetActorLocation()).SizeSquared() <
			(ExitGrate->GetComponentLocation() - Shooter->GetActorLocation()).SizeSquared();

		VentState->CrawlDirection = bEnteredVentFromEntryGrate ? CD_Forward : CD_Back;
		
		FVector EntryPoint;
		FVector EntryTangent;
		if (bEnteredVentFromEntryGrate)
		{
			EntryPoint = SplineComponent->GetWorldLocationAtDistanceAlongSpline(0.f);
			const FVector TangentSample = SplineComponent->GetWorldLocationAtDistanceAlongSpline(5.f);
			EntryTangent = (TangentSample - EntryPoint).GetSafeNormal2D();
		}

		else
		{
			EntryPoint = SplineComponent->GetWorldLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength());
			const FVector TangentSample = SplineComponent->GetWorldLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength() - 5.f);
			EntryTangent = (TangentSample - EntryPoint).GetSafeNormal2D();
		}

		FTransform EntryTransform;
		EntryTransform.SetLocation(EntryPoint);
		EntryTransform.SetRotation(FRotator(0.f, EntryTangent.ToOrientationRotator().Yaw, 0.f).Quaternion());
			
		Shooter->PlayCutsceneAnimation(
			Cast<UShooterAnimInstance>(Shooter->GetBodyMesh()->GetAnimInstance())->VentEnterMontage,
			EntryTransform,
			StateName);

		VentState->SetAnimState(true);
	}
}

#if WITH_EDITOR
void AVent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//turns off the vent during editor time when the details panel bool is ticked
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AVent, bDisabled))
	{
		SetVentDisabled(bDisabled);
	}	
}
#endif