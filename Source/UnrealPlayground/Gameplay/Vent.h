#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vent.generated.h"

class UStaticMeshComponent;

UCLASS()
class UNREALPLAYGROUND_API AVent : public AActor
{
	GENERATED_BODY()
	
public:	
	AVent();

protected:
	virtual void BeginPlay() override;

	/** How many seconds after taking damage do the lazers stay disabled for*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vent")
	float DisableDuration;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Frame;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Grate;

	/** Utilize overlap so we never close unless all objects have exited*/
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GrateTrigger;

	UPROPERTY(Category = Health, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* Health;

	/** Disables the grate for DisableDuration*/
	UFUNCTION()
	void DisableGrateForDuration();

	/** Turns the grate back on*/
	void ReEnableGrate();

	/** Calls above function if conditions are met*/
	void MaybeReEnableGrate();

	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:

	/** Need this in the event a pawn is blocking the vent when we are trying to enable the lazers (which would get the object stuck)*/
	FORCEINLINE bool ShouldEnable() { return OverlappedPawns == 0 && bIsDisabled && !bDelayRunning; }

	/** See grate trigger*/
	int32 OverlappedPawns;

	//these bools should be an enum state but i am lazy

	/** Is the grate currently disabled*/
	uint8 bIsDisabled : 1;

	/** Is the delayed action running*/
	uint8 bDelayRunning : 1;
};
