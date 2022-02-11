#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class UDelayedActionManager;
class AShooter;
class AShooterController;

UCLASS()
class PROJECTNOVA_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AShooterGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void Tick(float DeltaTime) override;

	UDelayedActionManager* GetDelayedActionManager() const { return DelayedActionManager; }

	void PostLogin(APlayerController* NewPlayer) override;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
	AShooter* Shooter;

	UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
	AShooterController* ShooterController;

private:

	UPROPERTY(Transient)
	UDelayedActionManager* DelayedActionManager;
};