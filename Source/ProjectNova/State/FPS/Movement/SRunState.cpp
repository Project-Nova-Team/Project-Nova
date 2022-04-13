#include "SRunState.h"

void USRunState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);

	MaxAppliedInputSpeed = Movement->RunMaxSpeed;
	MoveAcceleration = Movement->RunAcceleration;
}

void USRunState::OnEnter()
{
	Super::OnEnter();
	Input->bIsRunning = true;
}

void USRunState::OnExit()
{
	Super::OnExit();
	Input->bIsRunning = false;
}

void USRunState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckForWalkState();
}

void USRunState::CheckForWalkState()
{
	//This iterative checking makes the code look more redundant but is more performant 

	//Went airborne or not holding sprint
	if (!Movement->bIsOnGround || !Input->bIsTryingToSprint)
	{
		FlagTransition("Walking", 1);
		return;
	}

	//Moving too slow
	const float MinRunSpeedSquare = Movement->RunMinSpeed * Movement->RunMinSpeed;
	if (Movement->Velocity.SizeSquared2D() < MinRunSpeedSquare)
	{
		FlagTransition("Walking", 1);
		return;
	}

	//Look direction not in the same direction as walking
	if ((Shooter->GetCamera()->GetForwardVector() | Movement->Velocity.GetSafeNormal2D()) < Movement->RunLookMoveMinDot)
	{
		FlagTransition("Walking", 1);
		return;
	}
}