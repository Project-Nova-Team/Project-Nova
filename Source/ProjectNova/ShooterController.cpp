#include "ShooterController.h"
#include "ShooterHUD.h"
#include "Player/Shooter.h"
#include "GameFramework/GameModeBase.h"

void AShooterController::ReceivePause()
{
	//Inform the game mode to pause the game
	bool bPaused = IsPaused();
	SetPause(!bPaused);

	//Tell our HUD to put up/down the HUD menu
	ShooterHUD->SetPauseDisplay(bPaused);
}

void AShooterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetGenericTeamId(FGenericTeamId(0));

	//I find myself getting lost in source when figuring out when the hell HUD gets created
	//in NM_Standalone. For simplicities sake, I'm performing this check here to just create 
	//one if it does not already exist
	if (MyHUD == nullptr)
	{
		UWorld* World = GetWorld();

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save HUDs into a map

		ShooterHUD = GetWorld()->SpawnActor<AShooterHUD>(World->GetAuthGameMode()->HUDClass, SpawnInfo);
		ShooterHUD->Initialize();

		MyHUD = ShooterHUD;
	}

	else if(ShooterHUD == nullptr)
	{
		//Let this crash us otherwise. Always make shooter controller have this type of HUD
		ShooterHUD = Cast<AShooterHUD>(MyHUD);
		ShooterHUD->Initialize();
	}

	if (MyHUD != nullptr)
	{
		MyHUD->bShowHUD = true;
	}

	if (AShooter* Shooter = GetPawn<AShooter>())
	{
		BindingHandle = Shooter->InputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AShooterController::ReceivePause).GetHandle();
	}
}

void AShooterController::OnUnPossess()
{
	if (AShooter* Shooter = GetPawn<AShooter>())
	{
		Shooter->InputComponent->RemoveActionBindingForHandle(BindingHandle);
	}

	Super::OnUnPossess();

	if (MyHUD != nullptr)
	{
		MyHUD->bShowHUD = false;
	}
}

void AShooterController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (TeamID != NewTeamID)
	{
		TeamID = NewTeamID;
	}
}