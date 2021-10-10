#pragma once

#include "CoreMinimal.h"
#include "SEventState.h"
#include "SDeathState.generated.h"


UCLASS()
class UNREALPLAYGROUND_API USDeathState : public USEventState
{
	GENERATED_BODY()

public:
	USDeathState() { }
	~USDeathState() { }

	void OnEnter() override;
};
