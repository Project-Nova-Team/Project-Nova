#include "SDiveState.h"

void USDiveState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	TargetHalfHeight = Movement->CollisionRadius;
	TargetCameraHeight = -Movement->CollisionRadius * 0.5f;
	MoveAcceleration = Movement->ProneAcceleration;
	MaxAppliedInputSpeed = 0;
}

void USDiveState::OnEnter()
{
	Super::OnEnter();

	//Apply impulse
	FVector Impulse = Shooter->GetCamera()->GetForwardVector().GetSafeNormal2D() * Movement->DiveHorizontalImpulse;
	Impulse.Z = Movement->DiveVerticalImpulse;
	Movement->Velocity += Impulse;

	//Lift slighly in the air so the next ground check returns false
	Movement->bIsOnGround = false;
	const FVector CurrentLoc = Collider->GetComponentLocation();
	const FVector Adjustment = FVector(0, 0, FLOOR_DIST);
	Collider->SetWorldLocation(CurrentLoc + Adjustment, true);
}

void USDiveState::Tick(const float DeltaTime)
{
	Movement->bIsOnGround = IsOnGround();
	CalculateVelocity(DeltaTime);
	SetNewLocation(DeltaTime);

	if (Movement->bIsOnGround && !Movement->bIsInTuckTransition)
	{
		FlagTransition("Proning");
	}
}