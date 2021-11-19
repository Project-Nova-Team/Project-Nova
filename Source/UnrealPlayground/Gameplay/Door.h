#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Door.generated.h"

class UStaticMeshComponent;
class UChildActorComponent;
class ANavLinkProxy;
struct FDelayedActionHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoorEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDoorLockEvent, bool, bJustLocked);

UENUM()
enum EDoorState
{
	EDS_Closed,
	EDS_Open,
	EDS_Changing
};

UCLASS()
class UNREALPLAYGROUND_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoor();

	/** Sets the lock status of this door*/
	UFUNCTION(BlueprintCallable)
	void SetIsLocked(const bool Value);

	/** Should this door be opening*/
	FORCEINLINE bool ShouldOpen() const { return CurrentPawnCount > 0 && !bIsLocked && State == EDS_Closed; }

	/** Should this door be shutting. This catches the interesting edge case where a door gets locked while open*/
	FORCEINLINE bool ShouldClose() const { return State == EDS_Open && (CurrentPawnCount == 0 || bIsLocked); }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LeftSide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RightSide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Frame;

	/** The door will open when a pawn enters this region and close when all pawns have left it*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Trigger;

	/** C++ doesnt seem to play nice with child actor components... use this so AI can walk through doors*/
	UPROPERTY(BlueprintReadOnly, Category = "Nav Link", meta = (AllowPrivateAccess = "true"))
	ANavLinkProxy* NavLink;

	/** Whether or not the door is currently locked*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	uint8 bIsLocked : 1;

	/** Whether or not the door should start lockedd*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	uint8 bStartLocked : 1;

	/** How many seconds does the door take to open/close all the way*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorTransitionTime;

	/** How many units does either door side slide away from the origin*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorOpenDistance;

	/** Invoked by ActorBeginOverlap*/
	UFUNCTION()
	virtual void ActorStartOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Invoked by ActorEndOverlap*/
	UFUNCTION()
	virtual void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Close or open the door if the conditions are met*/
	virtual void MaybeChangeDoorState();

	/** This function is run by the delayed action manager to lerp the door positions over time*/
	void OverTimeTransition(const EDoorState TargetState);

	/** 
	 * Invoked when SetIsLocked changes lock status (providing the new lock state)
	 * This lets us tell blueprint subclass to update an emissive material
	 */
	UPROPERTY(BlueprintAssignable)
	FDoorLockEvent OnDoorLockStatusChange;

	/** Invoked when the door starts opening*/
	UPROPERTY(BlueprintAssignable)
	FDoorEvent OnDoorOpen;

	/** Invoked when the door starts closing*/
	UPROPERTY(BlueprintAssignable)
	FDoorEvent OnDoorClose;

private:

	/** How many pawns are currently overlapped with this actor*/
	int32 CurrentPawnCount;

	/** Live state of this door*/
	TEnumAsByte<EDoorState> State;

	/** Delayed action handle for ease of LERP*/
	FDelayedActionHandle* Handle;
};
