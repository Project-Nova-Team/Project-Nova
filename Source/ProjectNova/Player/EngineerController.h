#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "EngineerController.generated.h"

class AEngineerHUD;

UCLASS()
class PROJECTNOVA_API AEngineerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:

	/** Cached cast version of MyHUD for ease of use*/
	AEngineerHUD* EngineerHUD;

	/** Team ID used for AI threat/friend detection*/
	FGenericTeamId TeamID;

	/** Have the HUD properties already been initialized. This occurs on first possession*/
	uint8 bHudInitialized;

public:

	void ReceivePause();

	/// Begin APawn Interface ///

	void OnUnPossess() override;
	void OnPossess(APawn* InPawn) override;

	void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	FGenericTeamId GetGenericTeamId() const override { return TeamID; }	
};
