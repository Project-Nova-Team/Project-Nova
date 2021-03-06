#include "SVentState.h"
#include "../../../State/FPS/ShooterStateMachine.h"
#include "../../../Gameplay/Vent.h"
#include "../../../Animation/ShooterAnimInstance.h"

void USVentState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	BodyInstance = Cast<UShooterAnimInstance>(Shooter->GetBodyMesh()->GetAnimInstance());
	ArmsInstance = Cast<UShooterAnimInstance>(Shooter->GetArmsMesh()->GetAnimInstance());
}

void USVentState::OnEnter()
{
	Super::OnEnter();

	Shooter->GetAnchor()->SetRelativeLocation(Movement->VentCameraLocation);

	SplineLength = Vent->GetSpline()->GetSplineLength();
	CurrentProgress = CrawlDirection == CD_Forward ? 0.f : SplineLength;

	Shooter->GetCombat()->SetEnabled(false);
}

void USVentState::OnExit()
{
	Super::OnExit();

	Vent->bInUse = false;
	SetAnimState(false);

	Shooter->GetCombat()->SetEnabled(true);
}

void USVentState::Tick(float DeltaTime)
{
	MoveAlongSpline(DeltaTime);
	SetShooterRotation(DeltaTime);
}

void USVentState::SetAnimState(const bool bValue)
{
	BodyInstance->bIsInVent = bValue;
	ArmsInstance->bIsInVent = bValue;
}

void USVentState::MoveAlongSpline(float DeltaTime)
{
	Input->bIsMoving = FMath::Abs(Input->MoveY) > KINDA_SMALL_NUMBER;

	if (!Input->bIsMoving)
	{
		return;
	}

	const float TravelDirection = CrawlDirection == CD_Forward ? 1.f : -1.f;
	CurrentProgress += Input->MoveY * DeltaTime * Movement->VentSpeed * TravelDirection;

	FVector SplinePoint = Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(CurrentProgress);
	SplinePoint.Z += Shooter->GetCollider()->GetScaledCapsuleHalfHeight();
	Shooter->SetActorLocation(SplinePoint);

	//Check if player exited vent
	if (CurrentProgress < 0.f || CurrentProgress > SplineLength)
	{	
		FTransform ExitTransform;
		ExitTransform.SetLocation(Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(FMath::Clamp(CurrentProgress, 0.f, SplineLength)));
		ExitTransform.SetRotation(GetEndTangent().ToOrientationQuat());

		Shooter->PlayCutsceneAnimation(BodyInstance->VentExitMontage, ExitTransform);
	}
}

void USVentState::SetShooterRotation(float DeltaTime)
{
	const float Length = CrawlDirection == CD_Forward ? SplineLength : 0.f;

	if (FMath::Abs(CurrentProgress - Length) < 8.f)
	{
		const FRotator LookRotation = GetEndTangent().ToOrientationRotator();
		Shooter->SetActorRotation(FMath::RInterpTo(Shooter->GetActorRotation(), LookRotation, DeltaTime, Movement->VentTurnSpeed));
	}

	else
	{
		const float Offset = CrawlDirection == CD_Forward ? 5.f : -5.f;
		const float Position = FMath::Clamp(CurrentProgress + Offset, 0.f, SplineLength);
		const FVector TangentSample(Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(Position));
		const FVector CurrentLocation(Shooter->GetActorLocation() - FVector(0.f, 0.f, Shooter->GetCollider()->GetScaledCapsuleHalfHeight()));

		FRotator LookRotation = (TangentSample - CurrentLocation).GetSafeNormal2D().ToOrientationRotator();
		Shooter->SetActorRotation(FMath::RInterpTo(Shooter->GetActorRotation(), LookRotation, DeltaTime, Movement->VentTurnSpeed));
	}
}

FVector USVentState::GetEndTangent()
{
	FVector Tangent;

	if (CrawlDirection == CD_Forward)
	{
		Tangent = Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(SplineLength)
			- Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(SplineLength - 5.f);
	}

	else
	{
		Tangent = Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(0)
			- Vent->GetSpline()->GetWorldLocationAtDistanceAlongSpline(5.f);
	}

	return Tangent.GetSafeNormal2D();
}
