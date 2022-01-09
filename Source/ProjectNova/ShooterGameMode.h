#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "ShooterGameMode.generated.h"

class UDelayedActionManager;
class AShooter;
class AAICell;

/** On Pause Delegate*/
DECLARE_MULTICAST_DELEGATE(FPauseEvent);

UCLASS()
class PROJECTNOVA_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AShooterGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void Tick(float DeltaTime) override;

	UDelayedActionManager* GetDelayedActionManager() const { return DelayedActionManager; }

	// public in order to be bound to input delegate
	UFUNCTION()
	void PauseGame();

	FPauseEvent OnPause;

private:
	UPROPERTY(Transient)
	UDelayedActionManager* DelayedActionManager;

	/** Collection of all the AI cells in the level. Fetched once on level load*/
	UPROPERTY(Transient)
	TArray<AAICell*> AICells;
	
	UPROPERTY(Transient)
	AShooter* Player;

	/** Invoked when the player dies*/
	UFUNCTION()
	void PlayerDeath();

	//TODO parsing the actor hierarchy is and always will be a crime
	//We will be serializing AICells and the player in builds
//#if WITH_EDITOR
	template<class ActorClass>
	ActorClass* FindActor(UWorld* World)
	{
		return *TActorIterator<ActorClass>(World);
	}

	template<typename ActorClass>
	void FindAllActors(UWorld* World, TArray<ActorClass*>& Out)
	{
		for (TActorIterator<ActorClass> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}

	void LoadCells();
//#endif
};
