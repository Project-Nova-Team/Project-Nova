#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Sense.h"
#include "BaseAIController.generated.h"

class ABaseAI;

UCLASS()
class PROJECTNOVA_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseAIController();

	UFUNCTION(BlueprintCallable, Category = "AI")
	FORCEINLINE ABaseAI* GetAICharacter() { return GetPawn<ABaseAI>(); }
	
	/** 
	 * Sets load status and handles necessary changes to AI
	 * 
	 * @param	bLoadAI				The new loaded status of this AI
	 * @param	bRevive				If true, revive this AI if it was previously dead
	 */
	UFUNCTION()
	void SetLogicEnabled(const bool bEnableLogic);

protected:

	/** BT Asset that runs when this controller posses a valid pawn*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	/** Sets the enabled status of every sense on this AI's perception component*/
	void SetSensesEnabled(const bool bEnableSenses);

	/** Sets the enabled status of the behavior tree on this AI*/
	void SetBehaviorTreeEnabled(const bool bEnableTree);

	/** Determines the attitude of this AI to the supplied actor*/
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	/** This function is called when the AI dies. It disables all perception and the behavior tree*/
	UFUNCTION()
	virtual void OwnerDeath();

	/** This function is called when the AI revives. It re-enables perception and the BT*/
	UFUNCTION()
	virtual void OwnerRevive();


	///Perception Related Functions///

	/** 
	 * Function bound to AAIController OnTargetPerceptionUpdated
	 * Checks team attitude of supplied actor and fires relevant stimulus events
	 */
	UFUNCTION()
	void PerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** Blueprint event executed when an AI registers a hostile sight stimulus*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Spot Enemy"))
	void ReceiveSpotEnemy(AActor* Enemy, const FAIStimulus& Stimulus);

	/** Blueprint event executed when an AI registers a hostile audio stimulus*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Hear Enemy"))
	void ReceiveHearEnemy(AActor* Enemy, const FAIStimulus& Stimulus);

	/** Blueprint event executed when an AI registers a friendly sight stimulus*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Spot Friend"))
	void ReceiveSpotFriendly(AActor* Friend, const FAIStimulus& Stimulus);

private:

	/** Whether or not the AI logic is active which includes perception and behavior trees*/
	uint8 bLogicEnabled : 1;

	/** Delegate handle for load status changes*/
	FDelegateHandle Handle;

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
