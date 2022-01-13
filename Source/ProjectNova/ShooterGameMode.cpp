#include "ShooterGameMode.h"
#include "Utility/DelayedActionManager.h"
#include "ShooterController.h"
#include "ShooterHUD.h"
#include "Player/Shooter.h"
#include "Gameplay/HealthComponent.h"

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

	//Remove this at some point becuase its really slow. It also crashes us if theres not a shooter in the level
	if (Player == nullptr)
	{
		Player = FindActor<AShooter>(GetWorld());
		Player->GetHealth()->OnDeath.AddDynamic(this, &AShooterGameMode::PlayerDeath);
	}

	//GEngine->bSuppressMapWarnings = true;
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DelayedActionManager->TickRunningActions(DeltaTime);
}

void AShooterGameMode::PlayerDeath()
{
	//Do reset pipeline stuff here when player dies
	Cast<AShooterHUD>(Cast<APlayerController>(Player->GetController())->MyHUD)->ShowDeathScreen();
}

void AShooterGameMode::PauseGame()
{
	OnPause.Broadcast();
}