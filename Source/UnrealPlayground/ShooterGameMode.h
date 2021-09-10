#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class UDelayedActionManager;

UCLASS()
class UNREALPLAYGROUND_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AShooterGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void Tick(float DeltaTime) override;

	UDelayedActionManager* GetDelayedActionManager() const { return DelayedActionManager; }

private:
	UPROPERTY()
	UDelayedActionManager* DelayedActionManager;
};
