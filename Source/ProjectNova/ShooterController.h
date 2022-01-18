#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "ShooterController.generated.h"

class AShooterHUD;

UCLASS()
class PROJECTNOVA_API AShooterController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AShooterController() { }

	void OnUnPossess() override;
	void OnPossess(APawn* InPawn) override;

	void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	FGenericTeamId GetGenericTeamId() const override { return TeamID; }

private:

	/** Load order for controller/hud/gamemode is complicated so we just use this to check if its the first load sequence*/
	uint8 bHUDInit : 1;

	/** Team ID*/
	FGenericTeamId TeamID;
};
