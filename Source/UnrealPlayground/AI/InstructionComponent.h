#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "InstructionComponent.generated.h"

class USplineComponent;
class UBlackboardComponent;

UENUM()
enum EInstructionState
{
	Patrol,
	Search,
	Attack
};

UENUM()
enum ESearchMode
{
	NotSearching,
	Investigate,
	Track,
	Follow
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInstructionStateEvent, EInstructionState, NewState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPLAYGROUND_API UInstructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstructionComponent();

	/** Performs component set up*/
	void Initialize(UBlackboardComponent* InitBlackBoard);

	/** Returns the spline component attached to the PatrolActor if one exists*/
	USplineComponent* GetPatrolPath() const;

	/**
	 * Event invoked when the perception component senses a disturbance in the environment
	 *
	 * @param	Invoker				The actor who caused the stimulus
	 * @param	Stimulus			The actual stimulus that the controller picked up
	 */
	UFUNCTION()
	void OnStimulus(AActor* Invoker, FAIStimulus Stimulus);
	
	/** Returns the current state of the AI*/
	EInstructionState GetState() const { return State; }
	
	/** Returns the search mode of the AI*/
	ESearchMode GetSearchMode() const { return SearchMode; }

	/** Sets active state of the AI*/
	UFUNCTION(BlueprintCallable)
	void SetState(const EInstructionState NewState);

	/** Sets the search mode of the AI*/
	UFUNCTION(BlueprintCallable)
	void SetSearchMode(const ESearchMode NewMode);

	/** Event invoked when a stimulus requests a change in state*/
	UPROPERTY(BlueprintAssignable)
	FInstructionStateEvent OnInstructionStateChange;

	/** While in search mode follow, we check if its been long enough to stop following*/
	void TickFollow(const float CurrentTime);

	/** While in attack mode we need to check if we lost sight of the player too long ago*/
	void TickAttack(const float CurrentTime);

private:
	/** Shorthand pointer to the blackboard*/
	UBlackboardComponent* Blackboard;

	/** Current state of the the AI*/
	UPROPERTY(VisibleAnywhere, Category = "AI | State")
	TEnumAsByte<EInstructionState> State;

	/** Current search state of the the AI*/
	UPROPERTY(VisibleAnywhere, Category = "AI | State")
	TEnumAsByte<ESearchMode> SearchMode;

	//TODO delete me, we need to get the player a better way
	UPROPERTY(EditAnywhere, Category = "AI | State")
	AActor* Player;

#if WITH_EDITOR
	UPROPERTY(EditAnywhere, Category = "AI | Debug")
	uint8 bDebug : 1;
#endif

	//Begin Patrol

	/** Actor in the level that contains a spline component the AI will patrol*/
	UPROPERTY(EditAnywhere, Category = "AI | Patrol")
	AActor* PatrolActor;

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



	//Begin Search

	/** 
	 *	Sound sources will always be heard at their implicit volume if heard from less than the this distance.
	 *	We square this value so we wont have to sqrt the distance
	 */
	UPROPERTY(VisibleAnywhere, Category = "AI | Search")
	float MinimumPropagationDistance;

	/** Minimum threshhold of human hearing in P/1 m^2*/
	UPROPERTY(VisibleAnywhere, Category = "AI | Search")
	float MinimumIntensityThreshold;

	/** Any sounds heard with a volume less than this will be discarded and ignored*/
	UPROPERTY(VisibleAnywhere, Category = "AI | Search")
	float MinimumHearingThreshold;

	/**
	 * Everytime the AI hears a sound, its annoyance level will increase. As the AI becomes more annoyed
	 * It's search mode will become increasingly more aggressive until it decides to attack regardless of
	 * whether or not it found the player by sight. This value determines how many actions are required
	 * to increase the search modes aggression.
	 */
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	int32 AnnoyanceThreshold;

	/** 
	 * Minimum verbosity of the AI hearing a sound required to make the AI begin tracking player location
	 * Score is determined by volume of the sound and distance to the sound origin
	 * When tracking, the AI moves to the players location which is calculated once.
	 */
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float TrackScore;

	/**
	 * Minimum verbosity of the AI hearing a sound required to make the AI begin following the player
	 * Score is determined by volume of the sound and distance to the sound origin
	 * When following, the AI moves to the players location which is calculated continuously.
	 */
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float FollowScore;

	/** How many seconds the AI follows the player before giving up and going back to an investigation state*/
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float FollowDuration;

	/**
	 * Minimum verbosity of the AI hearing a sound required to make the AI attack
	 * Score is determined by volume of the sound and distance to the sound origin
	 */
	UPROPERTY(EditAnywhere, Category = "AI | Search")
	float AttackScore;

	/** 
	 * Current world location of the highest scored audio stimulus.
	 * AI will move to this location during Investigation
	 */
	FVector AudioInvestigationLocation;

	/** Current highest score of an investigation audio source*/
	float CurrentInvestigationScore;

	/**
	 * Current number of annoyances affecting the AI's search mode
	 * This value resets on state/mode change
	 */
	int32 CurrentAnnoyance;

	/** Last time since a follow action was executed*/
	float FollowBeginTime;

	/**
	 * Scores the relevance of an audio stimulus based on distance and volume
	 * 
	 * @param	SourceLocation				World space location of where the sound occured
	 * @param	Volume						Strength of the sound
	 */
	float CalculateAudioScore(const FVector SourceLocation, const float Volume) const;

	/** Sets state when hearing a sound from the patrol state*/
	void DetermineSearchFromPatrol(const FVector SourceLocation, const float IncomingScore);

	/** Sets state when hearing a sound from the search state*/
	void DetermineSearchFromSearch(const FVector SourceLocation, const float IncomingScore);

	//Begin Attack

	/** How many seconds after losing sight of the player does the AI remain in the attack state*/
	UPROPERTY(EditAnywhere, Category = "AI | Attack")
	float NoSightAttackDuration;

	/** Last time player was seen/touched/damaged by the player (usually seen)*/
	float LastAggressionTime;

	/** Set based on stimulus response*/
	uint8 bHasVisionOfPlayer : 1;
};
