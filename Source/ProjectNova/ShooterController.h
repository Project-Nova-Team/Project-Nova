#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterController.generated.h"

class AShooterHUD;

UCLASS()
class PROJECTNOVA_API AShooterController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterController() { }

	void OnUnPossess() override;
	void OnPossess(APawn* InPawn) override;

private:

	/** Load order for controller/hud/gamemode is complicated so we just use this to check if its the first load sequence*/
	uint8 bHUDInit : 1;
};
