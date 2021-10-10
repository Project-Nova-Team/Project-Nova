#pragma once

#include "CoreMinimal.h"
#include "../StateMachine.h"
#include "AIStateMachine.generated.h"

class UBehaviorTree;

UCLASS()
class UNREALPLAYGROUND_API UAIStateMachine : public UStateMachine
{
	GENERATED_BODY()

public:
	UAIStateMachine() { }
	~UAIStateMachine() { }

	void SetState(const FString Key) override { Super::SetState(Key); }
	virtual void Initialize(UObject* Context) override;

	void PostInitLoadTrees(const TMap<FString, UBehaviorTree*>& TreeMap);

	uint8 GetWeightAtKey(const FString Key);

	uint8 GetActiveWeight();
};
