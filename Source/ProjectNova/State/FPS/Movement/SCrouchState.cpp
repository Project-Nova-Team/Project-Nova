#include "SCrouchState.h"

void USCrouchState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	TargetHalfHeight = Movement->StandingHeight * 0.5f;
	TargetCameraHeight = Movement->CameraHeight * 0.5f;
	ExpectedHalfHeight = TargetHalfHeight;
	MoveAcceleration = Movement->CrouchAcceleration;
	MaxAppliedInputSpeed = Movement->CrouchMaxSpeed;
}

void USCrouchState::OnEnter()
{
	Super::OnEnter();
}

void USCrouchState::OnExit()
{
	Super::OnExit();
}

void USCrouchState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Movement->bIsInTuckTransition)
	{
		CheckForWalkState();
	}
}

void USCrouchState::CheckForWalkState()
{
	//Stand up if possible when going airborne
	if (!Movement->bIsOnGround && CheckIfStandUpIsValid(Movement->StandingHeight))
	{
		FlagTransition("Walking", 1);
		return;
	}

	//Stand up if possible when input is requested
	if (!Input->bIsTryingToCrouch && CheckIfStandUpIsValid(Movement->StandingHeight))
	{
		FlagTransition("Walking", 1);
		return;
	}
}