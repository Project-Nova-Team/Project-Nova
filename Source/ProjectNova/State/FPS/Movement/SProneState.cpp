#include "SProneState.h"

void USProneState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	TargetHalfHeight = Movement->CollisionRadius;
	TargetCameraHeight = -Movement->CollisionRadius * 0.5f;
	ExpectedHalfHeight = TargetHalfHeight;
	MoveAcceleration = Movement->ProneAcceleration;
	MaxAppliedInputSpeed = Movement->ProneMaxSpeed;
}

void USProneState::OnEnter()
{
	Super::OnEnter();
	Input->Stance = WFS_Proning;
}

void USProneState::OnExit()
{
	Super::OnExit();

	//even if we end up in crouch, crouches OnEnter will fix this
	Input->Stance = WFS_Standing;
}

void USProneState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Don't do any checks if we're not done laying down
	if (Movement->bIsInTuckTransition)
	{
		return;
	}

	CheckForWalkState();
	CheckForCrouchState();
}

void USProneState::CheckForWalkState()
{
	//Stand up if possible when going airborne
	if (!Movement->bIsOnGround && CheckIfStandUpIsValid(Movement->StandingHeight))
	{
		FlagTransition("Walking", 1);
		return;
	}

	//Stand up if possible when input is requested
	if (Input->bIsTryingToProne && CheckIfStandUpIsValid(Movement->StandingHeight))
	{
		FlagTransition("Walking", 1);
		return;
	}
}

void USProneState::CheckForCrouchState()
{
	//Switch to crouch if possible when input is requested
	if (Input->bIsTryingToCrouch && CheckIfStandUpIsValid(Movement->StandingHeight * 0.5f))
	{
		FlagTransition("Crouching", 1);
		return;
	}
}