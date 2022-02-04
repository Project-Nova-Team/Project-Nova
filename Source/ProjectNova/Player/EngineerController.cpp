#include "EngineerController.h"
#include "EngineerHUD.h"

void AEngineerController::ReceivePause()
{
	//Inform the game mode to pause the game
	bool bPaused = IsPaused();
	SetPause(!bPaused);

	//Tell our HUD to put up/down the HUD menu
	EngineerHUD->SetPauseDisplay(bPaused);
}

void AEngineerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetGenericTeamId(FGenericTeamId(0));

	if (MyHUD != nullptr)
	{
		MyHUD->bShowHUD = true;		
	}
}

void AEngineerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (MyHUD != nullptr)
	{
		MyHUD->bShowHUD = false;
	}
}

void AEngineerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Super implementation assigns the hud so this is always safe
	EngineerHUD = Cast<AEngineerHUD>(MyHUD);
	EngineerHUD->Initialize();
}

void AEngineerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (TeamID != NewTeamID)
	{
		TeamID = NewTeamID;
	}
}