#pragma once

#include "CoreMinimal.h"
#include "STuckState.h"
#include "SCrouchState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API USCrouchState : public USTuckState
{
	GENERATED_BODY()

public:
	USCrouchState() { }
	~USCrouchState() { }

	void Tick(const float DeltaTime) override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;

private:
	/**
	 * Changes state to walking if...
	 *
	 * Not on ground			AND its possible to stand up at this position
	 * Crouch input requested	AND its possible to stand up at this position
	 */
	void CheckForWalkState();

	/**
	 * Changes state to proning if...
	 *
	 * Grounded
	 * Prone input requested
	 */
	void CheckForProneState();
};
