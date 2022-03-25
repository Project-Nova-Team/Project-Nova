#include "ShooterGameMode.h"
#include "Utility/DelayedActionManager.h"
#include "ShooterController.h"
#include "ShooterHUD.h"
#include "Player/Shooter.h"
#include "Gameplay/HealthComponent.h"
#include "Gameplay/QuickTimeManager.h"
#include "UObject/ConstructorHelpers.h"

AShooterGameMode::AShooterGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = AShooterController::StaticClass();
	HUDClass = AShooterHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<AQuickTimeManager> ManagerClass(TEXT("/Game/Blueprints/Systems/BP_QuickTimeManager"));
	QuickTimeManagerClass = ManagerClass.Class;
}

void AShooterGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	//Super does not compile ?
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	DelayedActionManager = NewObject<UDelayedActionManager>();
	DelayedActionManager->Initialize();

	FActorSpawnParameters Params;
	Params.Name = TEXT("Quick Time Manager");
	QuickTimeManager = GetWorld()->SpawnActor<AQuickTimeManager>(QuickTimeManagerClass, Params);
	
	//If we can't figure out why "lighting needs rebuild" warnings wont go away, we can set this flag for builds
	//GEngine->bSuppressMapWarnings = true;
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DelayedActionManager->TickRunningActions(DeltaTime);
}

void AShooterGameMode::PostLogin(APlayerController* NewPlayer)
{
	ShooterController = Cast<AShooterController>(NewPlayer);

	if (ShooterController != nullptr)
	{	
		Shooter = ShooterController->GetPawn<AShooter>();

		if (Shooter)
		{
			QuickTimeManager->Init();
		}
	}
}
