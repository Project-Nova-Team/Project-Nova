#pragma once

#include "CoreMinimal.h"
#include "AIInvestigationState.h"
#include "AIInvestigateState.generated.h"

UCLASS()
class PROJECTNOVA_API UAIInvestigateState : public UAIInvestigationState
{
	GENERATED_BODY()

public:
	UAIInvestigateState() { }
	~UAIInvestigateState() { }

	void OnEnter() override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
};
