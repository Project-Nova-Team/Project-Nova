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

	/** Character mesh*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cutscene")
	USkeletalMeshComponent* Mesh;

	/** Cutscene Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cutscene")
	class UCameraComponent* Camera;

private:

	/** Handle for finish cutscene timer*/
	FTimerHandle TimerHandle;

	/** Handle for skeleton updates*/
	FTimerHandle SkeletalHandle;

	/** Transform of the shooter at the beginning of the cutscene*/
	FTransform EntryTransform;

	/** State to exit to*/
	FString ExitState;

	/** Timer for cutscene blends*/
	float Timer;

public:
	/** 
	 * Plays the provided animation as a cutscene
	 *	
	 * @param	Animation				The animation played in the cutscene
	 * @param	ExitState				The state we put the player into upon finishing the cutscene
	 * @param	StartingTransform		Location for the cutscene to take place
	 */
	void PlayCutscene(class UAnimMontage* Animation, const FString ExitState = TEXT("Walking"), const FTransform& StartingTransform = FTransform::Identity);

	/** Removes control from the player and moves the camera onto the cutscene rig*/
	void StartCinematic(const FTransform& StartingTransform = FTransform::Identity);

	/** Returns control back to the player*/
	void EndCinematic(const FString FinsihState);

	/** Plays the provided montage on the cutscene rig*/
	void PlayAnimation(class UAnimMontage* Animation);


protected:

	void FinishCutscene();

private:

	/** Interpolate position/rotation towards the blend point*/
	void BlendTowardsTransform(FTransform Transform);

	void ReenableSkeleton();
};