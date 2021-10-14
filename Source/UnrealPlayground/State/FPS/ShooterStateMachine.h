#pragma once

#include "CoreMinimal.h"
#include "../StateMachine.h"
#include "ShooterStateMachine.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UShooterStateMachine : public UStateMachine
{
	GENERATED_BODY()
	
public:
	UShooterStateMachine() { }
	~UShooterStateMachine() { }
	virtual void Initialize(UObject* Context) override;
	void SetState(const FString NewStateKey) override;

	UState* GetPreviousState() { return PreviousState; }

protected:
	/** 
	 * Key to the state this state machine was using before its last swap
	 * The starting value will be equal to the initial state value
	 */
	UState* PreviousState;
};
