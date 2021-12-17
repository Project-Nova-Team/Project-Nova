#pragma once

#include "CoreMinimal.h"
#include "SEventState.h"
#include "SLookState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API USLookState : public USEventState
{
	GENERATED_BODY()

public:
	virtual void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	virtual void Tick(const float DeltaTime) override;

protected:

	void RotateCameraFromInput(const float DeltaTime);

	/** Short hand pointer to the container class with all movement related variables*/
	UShooterMovementComponent* Movement;

	/** Short hand pointer to the container struct with all the input states*/
	FShooterInput* Input;
};
