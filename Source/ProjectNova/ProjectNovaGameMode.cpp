#include "ProjectNovaGameMode.h"
#include "Player/Engineer.h"
#include "Player/EngineerController.h"

AProjectNovaGameMode::AProjectNovaGameMode()
{
	PlayerControllerClass = AEngineerController::StaticClass();
}


void AProjectNovaGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	//DelayedActionManager = NewObject<UDelayedActionManager>();
	//DelayedActionManager->Initialize();
}

void AProjectNovaGameMode::PostLogin(APlayerController* NewPlayer)
{
	EngineerController = Cast<AEngineerController>(NewPlayer);

	if (EngineerController != nullptr)
	{
		Engineer = EngineerController->GetPawn<AEngineer>();
	}
}