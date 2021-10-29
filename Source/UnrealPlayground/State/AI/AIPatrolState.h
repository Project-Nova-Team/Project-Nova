#pragma once

#include "CoreMinimal.h"
#include "AIState.h"
#include "AIPatrolState.generated.h"


UCLASS()
class UNREALPLAYGROUND_API UAIPatrolState : public UAIState
{
	GENERATED_BODY()

public:
	void OnEnter() override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
};
