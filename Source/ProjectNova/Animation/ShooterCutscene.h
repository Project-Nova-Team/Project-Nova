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

public:
	void PlayCutscene(class UAnimMontage* Animation);

private:
	void FinishCutscene();
};
