#pragma once

#include "CoreMinimal.h"
#include "STuckState.h"
#include "SDiveState.generated.h"

UCLASS()
class PROJECTNOVA_API USDiveState : public USTuckState
{
	GENERATED_BODY()
public:

	USDiveState() { }
	~USDiveState() { }

	void OnEnter() override;

	void Tick(const float DeltaTime) override;
	void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
};
