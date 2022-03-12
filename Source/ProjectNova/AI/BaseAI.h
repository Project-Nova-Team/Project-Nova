#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseAI.generated.h"

class UHealthComponent;
class APatrolPath;

DECLARE_MULTICAST_DELEGATE_OneParam(FLoadEvent, const bool);

UCLASS()
class PROJECTNOVA_API ABaseAI : public ACharacter
{
	GENERATED_BODY()

public:

	ABaseAI();

	/** Multicast delegate that broadcasts when SetIsLoaded gets called*/
	FLoadEvent OnLoadStatusChanged;

protected:

	/** Health component of this AI Character*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	UHealthComponent* Health;

	/** Active load state of this AI. Unloaded AI are invisible and have no logic running*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetIsLoaded, BlueprintSetter = SetIsLoaded, Category = "AI | General")
	uint8 bIsLoaded : 1;

	/** The Path this AI will follow when in a Patrol state*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol")
	APatrolPath* Path;

	/**
	 *	How many points should be remembered by the AI following this path when in Random path mode.
	 *	An AI will forget about a point once they have travelled to this many other points.
	 *	AI will not travel to a point that is remembered
	 *	This value should at maximum be one LESS than the total number of points
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol")
	int32 PointMemoryCapacity;

	/** Current collection of points remembered by this AI*/
	UPROPERTY(BlueprintReadWrite, Transient)
	TArray<int32> RememberedPoints;

	/** Decides which direction along a path the AI follows when their Path is a track*/
	UPROPERTY(BlueprintReadWrite, Transient)
	uint8 bReversedOnPath : 1;

public:

	/**
	 * Executed upon receiving a notice than a QuickTime action was complete 
	 * 
	 * @param	bSucceeded				Was this AI victorious in this QT Action
	 * @param	bCompleted				Was this the final action in this QT Event
	 * @param	SuccessCount			Number of times in a given event this AI has succeeded
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void QuickTimeActionComplete(bool bSucceeded, bool bCompleted, int32 SuccessCount);

	/** Gets health component*/
	FORCEINLINE UHealthComponent* GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	TArray<int32>& GetRememberedPoints() { return RememberedPoints; }

	/** Gets current load state*/
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsLoaded() const { return bIsLoaded; }

	/**
	 * Changes load state of the AI.
	 * This disables the AI's collision, mesh, and controller logic
	 *
	 * @param	bLoadAI				The new load state of this AI
	 */
	UFUNCTION(BlueprintSetter)
	void SetIsLoaded(const bool bLoadAI);

protected:
	/** Executed when the this Character's health reaches 0*/
	UFUNCTION()
	void OnDeath();

	/** Executed when the this Character revives*/
	UFUNCTION()
	void OnRevive();

#if WITH_EDITOR
public:
	/** Lets us hide the AI when a designer marks an AI as unloaded in the details panel*/
	void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
#endif

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif
};



#if WITH_EDITOR
class UTexture2D;

//Static helper class providing sprites for ai state debug
class FBaseAIDebugHelper
{

public:
	static UTexture2D* Default();
	static UTexture2D* Disabled();
	static UTexture2D* Aggressive();
	static UTexture2D* Aggravated();
};
#endif

