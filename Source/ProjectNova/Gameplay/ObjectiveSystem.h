#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveSystem.generated.h"

class UObjectiveWidget;
class AShooter;

UENUM()
enum EObjectWaypointType
{
	WAYPOINT_Static,
	WAYPOINT_Actor
};

USTRUCT(BlueprintType)
struct PROJECTNOVA_API FObjective
{
	GENERATED_BODY()

public:

	FObjective();

	/** Text displayed on the UI widgets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText ObjectiveText;

	/** Whether or not this objective displays a waypoint*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	uint8 bUseWaypoint : 1;

	/** Should this waypoint follow an actor or remain in a fixed position*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	TEnumAsByte<EObjectWaypointType> WaypointType;

	/** World space positions of the waypoints if set to static mode*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	TArray<FVector> TargetLocations;

	/** Actor for the waypoint to follow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	AActor* TargetActor;

	/** How many units above the target position/actor should the waypoint hover*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	float HeightOffset;

	/** How many units away does the player have to be to start tracking the next waypoint*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	float DeactivationDistance;
};

UCLASS()
class PROJECTNOVA_API AObjectiveSystem : public AActor
{
	GENERATED_BODY()
	
public:	

	AObjectiveSystem();

	void Tick(float DeltaTime) override;

	/** Player character reference. Set by game mode on spawn*/
	AShooter* Player;

	UObjectiveWidget* Widget;

protected:

	FObjective ActiveObjective;

	uint8 bWaypointActive : 1;

	int WaypointIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBillboardComponent* ArrowSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTextRenderComponent* DistanceText;

public:

	/** Clears the active object from the screen*/
	UFUNCTION(BlueprintCallable, Category = "Objective System")
	void ClearWaypoint();

	UFUNCTION(BlueprintCallable, Category = "Objective System")
	void SetNewObjective(const FObjective& NewObjective);

	UFUNCTION(BlueprintCallable, Category = "Objective System")
	void ClearObjective();
};
