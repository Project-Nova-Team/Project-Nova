#include "ShooterController.h"
#include "ShooterHUD.h"

void AShooterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//MyHUD->bShowHUD = true;
}

void AShooterController::OnUnPossess()
{
	Super::OnUnPossess();
	//MyHUD->bShowHUD = false;
}