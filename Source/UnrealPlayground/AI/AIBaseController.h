#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseController.generated.h"

UCLASS()
class UNREALPLAYGROUND_API AAIBaseController : public AAIController
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
};
