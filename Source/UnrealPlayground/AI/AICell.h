#pragma once

#include "CoreMinimal.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "AIBaseController.h"
#include "AICell.generated.h"

class AShooter;

UCLASS()
class UNREALPLAYGROUND_API AAICell : public ANavMeshBoundsVolume
{
	GENERATED_BODY()

public:
	AAICell();

	void Tick(float DeltaTime) override;
	
	
	UFUNCTION()
	void RegisterAudioStimulus(FVector Location, float Volume);

	UFUNCTION(BlueprintCallable)
	int GetInvestigationCount() const { return InvestigationCount; }

	/** Sets the reference to the player, called by the game mode instance on InitGame*/
	void SetPlayer(AShooter* Value) { Player = Value; }

	/** Called by individual AI to inform an investigation has begun or ended*/
	UFUNCTION()
	void RegisterInvestigator(bool bStarted);

	/** Called by individual AI to inform the Cell of its aggression status*/
	UFUNCTION()
	void RegisterAggressor(bool bStarted);

protected:
	void BeginPlay() override;

private:

	/** 
	 * Collection of AI that belong in this cell
	 * This collection is filled once during begin play and is populated with any
	 * AI units that exist within the bounds of this actor
	 */
	UPROPERTY(VisibleAnywhere, Transient)
	TArray<AAIBaseController*> AIUnits;

	/** Past down by the game mode instance, this reference will then be given to every AI in the cell*/
	AShooter* Player;

	/** World space location of the current stimulus*/
	FVector CurrentStimulusLocation;

	/** How many AI belonging to this cell are currently tasked as an investigator*/
	int32 InvestigationCount;

	/** How many AI belonging to this cell are currently aggressive and contributing to Attacking the player*/
	int32 AgressorCount;

	/** Gets every AI unit inside the cell volume and marks it as belonging to this cell*/
	void SetAIUnits();

	/**
	 * Sets the state of every AI in the cell. An AI can be supplied to skip the state change
	 *
	 * @param	NewState			New state for the AI
	 * @param	SearchMode			New search mode of the AI
	 * @param	Ignore				AI Unit we skip setting the state on
	 * @param	bIgnoreLower		If true, each AI will only respect a change if it has a higher priority
	 */
	void SetAllUnitStates(const FString NewState, const bool bIgnoreLower = false, AAIBaseController* Ignore = nullptr);

	/** 
	 * Sets the state of every AI in the cell. A collection of AI may be supplied to skip state change
	 * 
	 * @param	NewState			New state for the AI
	 * @param	Ignore				Collection of Units to skip setting this state on
	 * @param	bIgnoreLower		If true, each AI will only respect a change if it has a higher priority
	 */
	void SetAllUnitStatesList(const FString NewState, const bool bIgnoreLower = false, const TSet<AAIBaseController*> Ignore = TSet<AAIBaseController*>());

	/**
	 *	Selects the nearest AI in the cell to an audio source to investigate
	 *	All other AI will enter an agitated patrol state while the investigator
	 *  executes its search
	 *
	 * @param	StimulusSource				World space position where the audio stimulus occured
	 * @param	Strength					Decibel value of the audio source, if this value is negative it gurantees investigation state
	 */
	UFUNCTION()
	void SignalCellStimulus(const FVector StimulusSource, const float Strength);
};
