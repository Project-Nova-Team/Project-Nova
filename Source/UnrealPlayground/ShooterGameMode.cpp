#include "ShooterGameMode.h"
#include "Utility/DelayedActionManager.h"
#include "ShooterController.h"
#include "ShooterHUD.h"

AShooterGameMode::AShooterGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = AShooterController::StaticClass();
	HUDClass = AShooterHUD::StaticClass();
}

void AShooterGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	//Super does not compile ?
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	DelayedActionManager = NewObject<UDelayedActionManager>();
	DelayedActionManager->Initialize();
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DelayedActionManager->TickRunningActions(DeltaTime);
}