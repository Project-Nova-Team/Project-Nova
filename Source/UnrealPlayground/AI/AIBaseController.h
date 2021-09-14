#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseController.generated.h"

class UAIPerceptionComponent;

UCLASS()
class UNREALPLAYGROUND_API AAIBaseController : public AAIController
{
	GENERATED_BODY()
public:
	AAIBaseController();

protected:
	void BeginPlay() override;

	UPROPERTY(Category = Controller, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* Perception;	
};
