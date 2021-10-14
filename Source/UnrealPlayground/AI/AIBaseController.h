#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIBaseController.generated.h"

class UAIPerceptionComponent;
class UAIStateMachine;
class AAICell;
class AAIBase;
class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIEvent, bool, bStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAISoundEvent, FVector, Location, float, Strength);

UCLASS()
class UNREALPLAYGROUND_API AAIBaseController : public AAIController
{
	GENERATED_BODY()

public:
	AAIBaseController();

	/** Shorthand way to set state without introducing needless dependencies*/
	UFUNCTION(BlueprintCallable)
	void SetState(const FString& Key);

	UFUNCTION(BlueprintCallable)
	void SetAggression(const bool Value);

	UFUNCTION(BlueprintCallable)
	void SetInvestigation(const bool Value);

	/** Resets any search related values*/
	virtual void ClearSearchParameters();

	/** Sets search parameters in blackboard*/
	virtual void SetSearchParameters(const bool bUseTargetLoc);

	virtual void DetermineSearch(const FVector SourceLocation, const float SourceVolume);

	UAIStateMachine* GetStateMachine() const { return StateMachine; }

	void Tick(float DeltaTime) override;

	/** Invoked when this AI's aggression status changes*/
	FAIEvent OnAggression;

	/** Invoked when this AI's investigation status changes*/
	FAIEvent OnInvestigation;

	/** Invoked when this AI hears a valid sound*/
	FAISoundEvent OnHeardSound;

	bool IsAggressive() const { return bAggressive; }

	bool IsInvestigating() const { return bInvestigating; }

	bool IsDead() const { return bIsDead; }

	bool CanSeeTarget() const { return bHasVisionOfTarget; }

	float GetLastAggressionTime() const { return LastAgressiveTime; }

	void SetLastAggressionTime(const float Time) { LastAgressiveTime = Time; }

	float GetLastFollowTime() const { return LastFollowTime; }

	void SetMoveLocation(const FVector Location);

	FVector GetInvestigationLocation() const { return InvestigationLocation; }

	void SetInvestigationLocation(const FVector Location) { InvestigationLocation = Location; }

	void SetCell(AAICell* NewCell) { Cell = NewCell; }

	UFUNCTION(BlueprintCallable)
	AAICell* GetCell() const { return Cell; }

	AActor* GetTarget() const { return Target; }

	void SetTarget(AActor* NewTarget) { Target = NewTarget; }

	void SetStartingSearch(const bool Value);

	/** How long does this AI remain aggressive for before returning to a non aggressive state*/
	UPROPERTY(EditAnywhere, Category = "AI | Attack")
	float AggressionDuration;

	/** How long does this AI follow the player for before returning to an investigation state*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float FollowDuration;

	UPROPERTY(VisibleAnywhere, Category = "AI | State")
	FString CurrentState;

protected:
	void BeginPlay() override;

	/** Actor this AI will attack when aggressive (usually the player)*/
	AActor* Target;

	/** The owning actor of this controller casted as an AIBase*/
	AAIBase* AIOwner;
	
	/** Cell this AI belongs to*/
	AAICell* Cell;

	UPROPERTY(Transient)
	UAIStateMachine* StateMachine;

	/**
	 * Current annoyance counter.
	 * AI who recieve many audio stimulus become annoyed.
	 * Annoyed AI will increase the aggressiveness of their search state
	 */
	int32 Annoyance;

	/**
	 * Current score of an investigation
	 * Scores are set by computing the relative stimulus strength of the observer
	 * This is used to make sure AI can change their investigation location if they
	 * observe a stronger stimulus
	 */
	float InvestigationScore;

	/** World space position where the investigation should take place*/
	FVector InvestigationLocation;

	/** Called when this AI takes damage so it can report the event to the perception system*/
	UFUNCTION()
	void Damaged(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** 
	 * Invoked when this controller's perception component recieves a stimulus
	 * This function determines the stimulus type then calls the appropriate
	 * React function which subclasses of this controller override
	 * 
	 * @param	Invoker				The Actor who invoked this stimulus (almost always the player)
	 * @param	Stimulus			The stimulus that was invoked
	 */
	UFUNCTION()
	virtual void DispatchStimulus(AActor* Invoker, FAIStimulus Stimulus);

	virtual void ReactToPhysical(AActor* const Invoker, const FAIStimulus& Stimulus);

	virtual void ReactToSight(AActor* const Invoker, const FAIStimulus& Stimulus);

	virtual void ReactToSound(AActor* const Invoker, const FAIStimulus& Stimulus);

	/** Selects what state to enter based on an audio signal while patrolling*/
	virtual void DetermineSearchInternal(const FVector& SourceLocation, const float IncomingScore);

	/** Package behavior trees and pass them to the state machine here*/
	virtual void LoadStateTrees();

	/** Computes raw decibel value into relative decibel volume using linear scale*/
	float CalculateAudioScore(const FVector& SourceLocation, float Volume);

	UPROPERTY(EditAnywhere, Category = "AI | Trees")
	UBehaviorTree* PatrolTree;

	UPROPERTY(EditAnywhere, Category = "AI | Trees")
	UBehaviorTree* AgitatedTree;

	UPROPERTY(EditAnywhere, Category = "AI | Trees")
	UBehaviorTree* InvestigationTree;

	UPROPERTY(EditAnywhere, Category = "AI | Trees")
	UBehaviorTree* TrackTree;

	UPROPERTY(EditAnywhere, Category = "AI | Trees")
	UBehaviorTree* FollowTree;

	UPROPERTY(EditAnywhere, Category = "AI | Trees")
	UBehaviorTree* AttackTree;
	
	/** The index point of the patrol path the AI is at*/
	UPROPERTY(VisibleAnywhere, Category = "AI | Patrol")
	int32 PatrolIndex;

	/**
	 * World space location the AI should return to if it does not have a patrol path.
	 * If 0,0,0 the AI will set its spawn position as its home instead when play starts
	 */
	UPROPERTY(EditAnywhere, Category = "AI | Patrol")
	FVector Home;

	/** A spline representing the path this AI will try to patrol*/
	USplineComponent* PatrolPath;

	/** Whether or not the AI will walk in reverse order of the points when on a non looped path*/
	uint8 bIsReversedOnPath : 1;

	/** Returns the position of the next patrol path waypoint the AI will try to navigate towards*/
	UFUNCTION(BlueprintCallable)
	FVector GetNavigablePatrolPoint() const;

	/** Returns the spline point index that comes after StartingPoint*/
	UFUNCTION(BlueprintCallable)
	int32 GetNextPatrolPoint(const int32 StartingPoint) const;

	/** Sets the patrol index to the next point on the patrol path*/
	UFUNCTION(BlueprintCallable)
	void UpdatePatrolIndex();

	/** How loud does a sound need to be for an AI to investigate it*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float InvestigateVolume;

	/** How loud does a sound need to be for an AI to track the target*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float TrackVolume;

	/** How loud does a sound need to be for an AI to follow the target*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float FollowVolume;

	/** A sound heard from within this distance will always be at max volume*/
	UPROPERTY(VisibleAnywhere, Category = "AI | Search")
	float MinimumPropagationDistance;

	/** Real world minimum audible sound volume in proper P/A form*/
	UPROPERTY(VisibleAnywhere, Category = "AI | Search")
	float MinimumIntensityThreshold;

	/** Any sounds heard that are quieter than this will be ignored*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float MinimumHearingThreshold;

	/** If the AI annoyance counter reaches this value they will increase search aggression*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	int32 AnnoyanceThreshold;

	/** Cleans up and pauses BT logic and perceptual stimulus reponse*/
	UFUNCTION()
	void OwnerDeath() { SetLifeStatus(false); }

	/** 
	 * Sets the activity of perceptual stimulus and also informs the AIBase character to reset its components
	 * 
	 * @param	bIsAlive				New status for the AI. True if the AI is respawning, False if it is dying
	 */
	void SetLifeStatus(const bool bIsAlive);
	
	/**
	 * Is this AI agressive and trying to attack its target
	 * Note this is not the same as its state AI can still be in attack mode
	 * even if it not aggressive. It may just be attacking because nearby AI are
	 **/
	uint8 bAggressive : 1;

	/** Is this AI investigating a disturbance*/
	uint8 bInvestigating : 1;

	/** Was the last visual stimulus a success*/
	uint8 bHasVisionOfTarget : 1;

	uint8 bIsDead : 1;

	/** Game time the target last registered an agression*/
	float LastAgressiveTime;

	/** Game time the target last started following the target*/
	float LastFollowTime;
};
