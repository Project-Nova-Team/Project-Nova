#pragma once

#include "CoreMinimal.h"
#include "SMovementState.h"
#include "SWalkState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API USWalkState : public USMovementState
{
	GENERATED_BODY()

public:
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void Tick(const float DeltaTime) override;

private:

	/**
	 * Changes state to running if...
	 *
	 * Grounded
	 * Sprint input is received
	 * Current speed is in above near max walk speed
	 * Look direction is close enough to current movement direction
	 */
	void CheckForRunState();

	/**
	 * Changes state to crouching if...
	 *
	 * Grounded
	 * Crouch input is received
	 */
	void CheckForCrouchState();	

	/**
	 * Changes state to proning if...
	 *
	 * Grounded
	 * Prone input is received
	 */
	void CheckForProneState();
};
