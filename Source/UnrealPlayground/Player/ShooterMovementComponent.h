#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ShooterMovementComponent.generated.h"

/** 
 * This component contains values used for gameplay 
 * Designers can access all the properties from the shooter blueprint and adjust accordingly
 * USMovementState and its children read these values to perform game logic
 */


struct FBaseInfo
{
	/** The collider we are standing on*/
	UPrimitiveComponent* BaseObject;

	/** If this base can move and if so should we update position when standing on it*/
	uint8 bIsDynamic : 1;

	/** Position of the base last tick*/
	FVector OldLocation;
};

UCLASS()
class UNREALPLAYGROUND_API UShooterMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UShooterMovementComponent();

	/** Whether or not the collider is positioned on the ground*/
	UPROPERTY(VisibleAnywhere, Category = "Control | State")
		uint8 bIsOnGround : 1;

	/** Current state of the shooter*/
	UPROPERTY(VisibleAnywhere, Category = "Control | State")
		FString State;

	/** Controls the speed at which the camera turns*/
	UPROPERTY(EditAnywhere, Category = "Control | Camera")
		float CameraSensitivity;

	/** The max angle in degrees the camera can look up with respect to the XY plane*/
	UPROPERTY(EditAnywhere, Category = "Control | Camera", meta = (ClampMin = "0.1", ClampMax = "89.9"))
		float CameraMaxAngle;

	/** The min angle in degrees the camera can look down with respect to the XY plane*/
	UPROPERTY(EditAnywhere, Category = "Control | Camera", meta = (ClampMin = "-89.9", ClampMax = "-0.1"))
		float CameraMinAngle;

	/** The relative height at which the camera joint rests on the collider */
	UPROPERTY(EditAnywhere, Category = "Control | Camera")
		float CameraHeight;

	/** How far away can we be from objects like weapons and buttons to interact with them?*/
	UPROPERTY(EditAnywhere, Category = "Control | Camera")
		float InteractionDistance;

	/** The maximum angle a slope can be for the shooter to walk on it*/
	UPROPERTY(EditAnywhere, Category = "Control | General")
		float MaxSurfaceAngle;

	/** Max height of a surface collided with for us to step up on it*/
	UPROPERTY(EditAnywhere, Category = "Control | General")
		float MaxStepHeight;

	/** The half height of the shooter's collider when standing */
	UPROPERTY(EditAnywhere, Category = "Control | General")
		float StandingHeight;

	/** The radius of the shooter's collider*/
	UPROPERTY(EditAnywhere, Category = "Control | General")
		float CollisionRadius;

	/** Determines how much the current surface's friction applies*/
	UPROPERTY(EditAnywhere, Category = "Control | General", meta = (ClampMin = "0"))
		float FrictionMultiplier; 

	/** Multiplier for friction applied when changing input direction, higher values yield snappier movement*/
	UPROPERTY(EditAnywhere, Category = "Control | General", meta = (ClampMin = "0"))
		float FrictionTurnMultiplier;

	/** Gravity modifier*/
	UPROPERTY(EditAnywhere, Category = "Control | General")
		float GravityMultiplier;

	/** Controls how much control input has on the shooter while airborne*/
	UPROPERTY(EditAnywhere, Category = "Control | General", meta = (ClampMin = "0.0", ClampMax = "1"))
		float AirControlMultiplier;

	/** How much vertical velocity is added to the shooter when jumping*/
	UPROPERTY(EditAnywhere, Category = "Control | General", meta = (ClampMin = "0.0"))
		float JumpForce;

	/** The max magnitude of velocity that can be applied to the shooter on the XY plane*/
	UPROPERTY(EditAnywhere, Category = "Control | General", meta = (ClampMin = "0.0"))
		float MaxHorizontalSpeed;

	/** The max magnitude of velocity that can be applied to the shooter on the Z axis*/
	UPROPERTY(EditAnywhere, Category = "Control | General", meta = (ClampMin = "0.0"))
		float MaxVerticalSpeed;

	/** The number of units the shooter accelerates each second while walking*/
	UPROPERTY(EditAnywhere, Category = "Control | Walk")
		float WalkAcceleration;

	/** The max number of units the shooter moves each second while walking*/
	UPROPERTY(EditAnywhere, Category = "Control | Walk")
		float WalkMaxSpeed;

	/** The number of units the shooter accelerates each second while running*/
	UPROPERTY(EditAnywhere, Category = "Control | Run")
		float RunAcceleration;

	/** The max number of units the shooter moves each second while running*/
	UPROPERTY(EditAnywhere, Category = "Control | Run")
		float RunMaxSpeed;

	/** The minimum number of units the shooter must move each second to maintain or enter the running state*/
	UPROPERTY(EditAnywhere, Category = "Control | Run")
		float RunMinSpeed;

	/** 
	 * This controls how close the movement and look direction have to be to maintain a running state
	 * A value of 1 means they must be perfectly aligned and a value of -1 means the look direction does not matter
	 */
	UPROPERTY(EditAnywhere, Category = "Control | Run", meta = (ClampMin = "-1", ClampMax = "1"))
		float RunLookMoveMinDot;

	/** The number of units the shooter accelerates each second while crouching*/
	UPROPERTY(EditAnywhere, Category = "Control | Crouch")
		float CrouchAcceleration;

	/** The max number of units the shooter moves each second while crouching*/
	UPROPERTY(EditAnywhere, Category = "Control | Crouch")
		float CrouchMaxSpeed;

	/** The amount of time in seconds it takes to crouch down*/
	UPROPERTY(EditAnywhere, Category = "Control | Crouch")
		float CrouchTime;

	/** The number of units the shooter accelerates each second while proning*/
	UPROPERTY(EditAnywhere, Category = "Control | Prone")
		float ProneAcceleration;

	/** The max number of units the shooter moves each second while proning*/
	UPROPERTY(EditAnywhere, Category = "Control | Prone")
		float ProneMaxSpeed;

	/** The amount of time crouch input needs to be held for the shooter to go prone*/
	UPROPERTY(EditAnywhere, Category = "Control | Prone")
		float ProneInputTime;

	/** The minimum amount of speed required to dive forward*/
	UPROPERTY(EditAnywhere, Category = "Control | Dive")
		float DiveMinSpeed;

	/** The amount of vertical velocity added to the shooter when entering a dive*/
	UPROPERTY(EditAnywhere, Category = "Control | Dive")
		float DiveVerticalImpulse;

	/** The amount of horizontal velocity added to the shooter when entering a dive*/
	UPROPERTY(EditAnywhere, Category = "Control | Dive")
		float DiveHorizontalImpulse;

	/** Set to true if shooter overlaps vault trigger and is looking at vault object*/
	UPROPERTY(BlueprintReadWrite)
	uint8 bCanVault : 1;

	/** Information regarding the surface the shooter is currently standing on*/
	FHitResult GroundHitData;

	/** Input relative to the camera's forward vector*/
	FVector CameraRelativeInput;

	/** The velocity applied to the shooter from input*/
	FVector InputVelocity;

	/** Whether or not this state is currently adjusting the collider height*/
	uint8 bIsInTuckTransition : 1;

	//bad var name technically this is the minslopez
	/** The maximum Z component of the ground impact normal for the player to be grounded */
	float MaxSlopeZ;

	/** Information regarding the floor we are standing on and whether we should move with it*/
	FBaseInfo Base;

protected:
	virtual void BeginPlay() override;
};
