#pragma once

#include "CoreMinimal.h"
#include "AIState.h"
#include "AIAttackState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UAIAttackState : public UAIState
{
	GENERATED_BODY()

public:

	UAIAttackState() { }
	~UAIAttackState() { }

	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	void OnEnter() override;
	void OnExit() override;
	void Tick(const float DeltaTime) override;
};
