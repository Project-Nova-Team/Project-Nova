#include "SCrouchState.h"

void USCrouchState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
	TargetHalfHeight = Movement->StandingHeight * 0.5f;
	TargetCameraHeight = Movement->CameraHeight * 0.5f;
	MoveAcceleration = Movement->CrouchAcceleration;
	MaxAppliedInputSpeed = Movement->CrouchMaxSpeed;
}

void USCrouchState::OnEnter()
{
	Super::OnEnter();
	Input->Stance = WFS_Crouching;
}

void USCrouchState::OnExit()
{
	Super::OnExit();
	Input->Stance = WFS_Standing;
}

void USCrouchState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Don't do any checks if we're not done crouching down
	if (Movement->bIsInTuckTransition)
	{
		return;
	}

	CheckForWalkState();
	CheckForProneState();
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
	if (Input->bIsTryingToCrouch && CheckIfStandUpIsValid(Movement->StandingHeight))
	{
		FlagTransition("Walking", 1);
		return;
	}
}

void USCrouchState::CheckForProneState()
{
	//Switch to prone if on ground and input is requested
	if (Movement->bIsOnGround && Input->bIsTryingToProne)
	{
		FlagTransition("Proning", 1);
		return;
	}
}

