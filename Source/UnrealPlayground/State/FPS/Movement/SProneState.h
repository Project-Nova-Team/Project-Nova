#pragma once

#include "CoreMinimal.h"
#include "STuckState.h"
#include "SProneState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API USProneState : public USTuckState
{
	GENERATED_BODY()

public:

	USProneState() { }
	~USProneState() { }

	void Tick(const float DeltaTime) override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;

private:

	/**
	 * Changes state to walking if...
	 *
	 * Not on ground			AND its possible to stand up at this position
	 * Prone input requested	AND its possible to stand up at this position
	 */
	void CheckForWalkState();

	/**
	 * Changes state to crouching if...
	 *
	 * Crouch input requested	AND its possible to stand up at this position
	 */
	void CheckForCrouchState();
};
