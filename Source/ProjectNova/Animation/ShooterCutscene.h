#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterCutscene.generated.h"

UCLASS()
class PROJECTNOVA_API AShooterCutscene : public AActor
{
	GENERATED_BODY()
	
public:	
	AShooterCutscene();

	class AShooter* Shooter;

protected:
	/** Number of seconds to blend between cameras*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutscene")
	float BlendTime;

	/** Arms mesh*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cutscene")
	USkeletalMeshComponent* Arms;

	/** Torso and legs*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cutscene")
	USkeletalMeshComponent* Body;

	/** Cutscene Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cutscene")
	class UCameraComponent* Camera;

private:

	/** Handle for finish cutscene timer*/
	FTimerHandle TimerHandle;

	/** Transform of the shooter at the beginning of the cutscene*/
	FTransform EntryTransform;

	/** Timer for cutscene blends*/
	float Timer;

public:
	/** 
	 * Plays the provided animation as a cutscene
	 *	
	 * @param	Animation				The animation played in the cutscene
	 * @param	ExitState				The state we put the player into upon finishing the cutscene
	 */
	void PlayCutscene(class UAnimMontage* Animation, const FString& ExitState = TEXT("Walking"), const FTransform& StartingTransform = FTransform::Identity);

private:
	/** Can't use a reference since string goes out of scope*/
	void FinishCutscene(const FString ExitState);

	/** Returns control to the player after the cutscene finishes*/
	void BlendComplete(const FString ExitState);

	/** Interpolate position/rotation towards the blend point*/
	void BlendTowardsTransform(FTransform Transform);
};