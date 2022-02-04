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

	if (MyHUD == nullptr)
	{
		//No HUD, create one.
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save HUDs into a map
		EngineerHUD = GetWorld()->SpawnActor<AEngineerHUD>(SpawnInfo);
		MyHUD = EngineerHUD;
	}

	if (EngineerHUD != nullptr && !bHudInitialized)
	{
		MyHUD->bShowHUD = true;

		if (!bHudInitialized)
		{
			EngineerHUD->Initialize();
			bHudInitialized = true;		
		}	
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

void AEngineerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (TeamID != NewTeamID)
	{
		TeamID = NewTeamID;
	}
}