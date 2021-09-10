#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterController.generated.h"

class AShooterHUD;

UCLASS()
class UNREALPLAYGROUND_API AShooterController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterController() { }

	void OnUnPossess() override;
	void OnPossess(APawn* InPawn) override;
};
