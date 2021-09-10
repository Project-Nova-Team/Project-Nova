#include "ShooterController.h"
#include "ShooterHUD.h"

void AShooterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

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