#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "SpawnAITrigger.generated.h"

class AAIBase;

UCLASS()
class PROJECTNOVA_API ASpawnAITrigger : public ATrigger
{
	GENERATED_BODY()

protected:

	/**
	 * When triggered, any AI who are in this collection will be become active and begin roaming their patrol path if one exists.
	 * If an AI in this collection is dead, it will respawn at its starting location if RespawnAtCurrentLocation is false or at its current location if true.
	 * If the AI is still alive, nothing will happen and the AI will continue doing whatever it was previously doing.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	TArray<AAIBase*> SpawnCollection;
	

	/** If true, dead AI that respawn will begin patrolling from their current location. Otherwise they will spawn where they were originally placed in the level*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	uint8 bRespawnAtCurrentLocation : 1;

	void ExecuteTrigger(APawn* Sender) override;
};
