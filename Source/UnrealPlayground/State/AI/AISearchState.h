#pragma once

#include "CoreMinimal.h"
#include "AIState.h"
#include "AISearchState.generated.h"

UCLASS()
class UNREALPLAYGROUND_API UAISearchState : public UAIState
{
	GENERATED_BODY()

public:
	void OnEnter() override;
};
