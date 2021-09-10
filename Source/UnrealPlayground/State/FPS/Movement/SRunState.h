#pragma once

#include "CoreMinimal.h"
#include "SMovementState.h"
#include "SRunState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API USRunState : public USMovementState
{
	GENERATED_BODY()
	
public:
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void Tick(const float DeltaTime) override;

private:
	/**
	 * Changes state to walking if...
	 *
	 * Sprint input is not being received
	 * The direction of input varies too far from the current movement vector
	 * Current speed is above near max walk speed
	 * Shooter goes airborne
	 */
	void CheckForWalkState();

	/**
	 * Changes state to diving if...
	 *
	 * Prone input is being received
	 * Current speed is at a near maximum
	 */
	void CheckForDiveState();
};
