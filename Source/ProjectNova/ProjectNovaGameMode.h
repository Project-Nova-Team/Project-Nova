#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectNovaGameMode.generated.h"

class AEngineer;
class AEngineerController;

UCLASS()
class PROJECTNOVA_API AProjectNovaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AProjectNovaGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void PostLogin(APlayerController* NewPlayer);

	UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
	AEngineer* Engineer;

	UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
	AEngineerController* EngineerController;
};