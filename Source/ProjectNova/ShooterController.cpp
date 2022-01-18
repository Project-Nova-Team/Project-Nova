#include "ShooterController.h"
#include "ShooterHUD.h"

void AShooterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetGenericTeamId(FGenericTeamId(0));

	if (MyHUD != nullptr)
	{
		MyHUD->bShowHUD = true;

		if (!bHUDInit)
		{
			bHUDInit = true;
			AShooterHUD* HUD = Cast<AShooterHUD>(MyHUD);
			HUD->Initialize();
		}
	}
}

void AShooterController::OnUnPossess()
{
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