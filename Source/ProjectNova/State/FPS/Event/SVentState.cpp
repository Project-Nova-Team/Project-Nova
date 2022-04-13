#include "SVentState.h"
#include "../../../State/FPS/ShooterStateMachine.h"
#include "../../../Gameplay/Vent.h"
#include "../../../Animation/ShooterAnimInstance.h"

void USVentState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	AnimInstance = Cast<UShooterAnimInstance>(Shooter->GetBodyMesh()->GetAnimInstance());
}

void USVentState::OnEnter()
{
	Super::OnEnter();

	Vent->bInUse = true;
	AnimInstance->bIsInVent = true;

	SplineLength = Vent->GetSpline()->GetSplineLength();
	CurrentProgress = CrawlDirection == CD_Forward ? 0.f : 1.f;
}

void USVentState::OnExit()
{
	Super::OnExit();

	Vent->bInUse = false;
	AnimInstance->bIsInVent = false;

	Shooter->SetActorRotation(FRotator(0.f, Shooter->GetActorRotation().Yaw, 0.f));
}

void USVentState::Tick(float DeltaTime)
{
	MoveAlongSpline(DeltaTime);
	//SetShooterRotation(DeltaTime);
}

void USVentState::MoveAlongSpline(float DeltaTime)
{
	Input->bIsMoving = Input->MoveY > KINDA_SMALL_NUMBER;

	if (!Input->bIsMoving)
	{
		return;
	}

	const float TravelDirection = CrawlDirection == CD_Forward ? 1.f : -1.f;
	CurrentProgress += Input->MoveY * DeltaTime * Movement->VentSpeed * TravelDirection;

	Shooter->SetActorLocation(Vent->GetSpline()->GetLocationAtTime(CurrentProgress, ESplineCoordinateSpace::World));

	//Check if player exited vent
	if (CurrentProgress < 0.f || CurrentProgress > 1.f)
	{	
		FTransform ExitTransform;
		ExitTransform.SetLocation(
			Vent->GetSpline()->GetLocationAtTime(FMath::Clamp(CurrentProgress, 0.f, 1.f), ESplineCoordinateSpace::World)
			+ FVector(0.f, 0.f, Shooter->GetShooterMovement()->StandingHeight));
		ExitTransform.SetRotation(Shooter->GetActorRotation().Quaternion());
		Shooter->PlayCutsceneAnimation(AnimInstance->VentExitMontage, ExitTransform);
	}
}

void USVentState::SetShooterRotation(float DeltaTime)
{
	const float Offset = (CrawlDirection == CD_Forward ? 1 : -1) * 0.01f;
	const FVector TangentSample(Vent->GetSpline()->GetLocationAtTime(CurrentProgress + Offset, ESplineCoordinateSpace::World).GetSafeNormal2D());
	
	//This is capable of changing pitch which can allow crawling up/down slopes in a vent
	//Have to be considerate of undoing any pitch to the actor after exiting the vent
	FRotator LookRotation = (Shooter->GetActorLocation() - TangentSample).ToOrientationRotator();
	Shooter->SetActorRotation(FMath::RInterpTo(Shooter->GetActorRotation(), LookRotation, DeltaTime, Movement->VentTurnSpeed));
}
