//Copyright 2021 Brendan Lienau. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../ShooterState.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "DrawDebugHelpers.h"
#include "SMovementState.generated.h"

/** Magic numbers*/
#define GROUND_CHECK_EPSILON (0.995f) 
#define FLOOR_DIST (2.f)
#define FORCE_OF_GRAVITY (981.f)
#define INPUT_EPSILON (1.001f)

UCLASS()
class PROJECTNOVA_API USMovementState : public UShooterState
{
	GENERATED_BODY()

public:
	USMovementState() { }
	~USMovementState() { }

	virtual void Initialize(UStateMachine* StateMachine, UObject* ContextObject) override;
	virtual void OnEnter() override;
	virtual void OnExit() override { }
	virtual void Tick(const float DeltaTime) override;

	/** The resting half height value of the shooter's collider in this state*/
	float ExpectedHalfHeight;

protected:
	/** The magnitude of velocity applied to the shooter each second input is received*/
	float MoveAcceleration;

	/** The maximum magnitude of velocity that can be applied to the shooter from input*/
	float MaxAppliedInputSpeed;

	/** Converts movement input relative to camera direction and adds movement vector*/
	virtual FVector ConvertInputRelativeToCamera() const;

	/** Rotates the camera to face the desired direction*/
	virtual void RotateCameraFromInput(const float DeltaTime);

	/** Applies upward velocity to the shooter if its grounded and jump input is recieved*/
	virtual void CheckForVault();

	/** Calculates the vector applied to the shooter based on gravity, friction, and input*/
	void CalculateVelocity(const float DeltaTime) const;

	/** 
	 * Applies a force of friction to the given vector
	 *
	 * @param	InOutVelocity				The velocity vector that will have friction applied to it
	 * @param	MinPostFrictionValue	The minimum magnitude of InVelocity after friction is applied
	 * @param	Friction				The scalar value used to determine how strong friction is
	 * @param	DeltaTime				Time slice for this function
	 */
	void ApplyFrictionToVelocity(FVector& InOutVelocity, const float MinPostFrictionValue, const float Friction, const float DeltaTime) const;

	/** 
	 * Projects the delta movement vector to run parallel with the surface provided
	 * 
	 * @param	DeltaMovement			The movement vector for this Tick
	 * @param	SurfaceHit				The surface DeltaMovement is projected onto				
	 */
	virtual FVector CalculateSurfaceMovement(FVector DeltaMovement, const FHitResult SurfaceHit) const;
	
	/** Attempts to move the shooter and runs any collision correction if necessary*/
	virtual void SetNewLocation(const float DeltaTime) const;

	/** 
	 * Moves the shooter along a delta and returns any collision info
	 * 
	 * @param	DeltaMovement			The change in position applied to the shooter this Tick
	 * @param	OutCollisionResult		Collision data from this move attempt
	 * @param	bAdjustZ				Whether or not adjust collider height off the ground on this move
	 */
	virtual void MoveShooter(const FVector DeltaMovement, FHitResult& OutCollisionResult, const bool bAdjustZ = true) const;

	/**
	 * Adjusts the position of the shooter when moving into barriers
	 *
	 * @param	DeltaTime				Time slice for this function
	 * @param	DeltaMovement			The change in position applied to the shooter this Tick
	 * @param	CollisionHit			The sweep result from the attempted move to position by the collider
	 */
	virtual void CorrectCollision(const float DeltaTime, FVector DeltaMovement, FHitResult& CollisionHit) const;

	/**
	 * Adjusts the shooter's position after colliding with a new surface
	 * 
	 * @param	DeltaMovement			The change in position applied to the shooter this Tick
	 * @param	CollisionHit			The sweep result from the attempted move to position by the collider
	 */
	virtual void NewInclineAdjust(FVector DeltaMovement, FHitResult& CollisionHit) const;

	/**
	 * Reduces the shooter's velocity to run tangent along the surface that is collided with
	 * 
	 * @param	InOutVelocity			The velocity vector that will be modified to run tangent to the collision normal
	 * @param	CollisionNormal			The normal of the collision
	 */
	virtual void CorrectVelocityAlongCollision(FVector& InOutVelocity, const FVector CollisionNormal) const;

	/**
	 * Evaluates if the shooter can step up on the surface it collided with
	 * 
	 * @param	CollisionData			The hit result of the most recent failed move
	 * @param	OutStepPosition			The world space position of where the shooter would end up if a step succeeds
	 */
	virtual bool CanStepOntoSurface(const FHitResult CollisionData, FVector& OutStepPosition) const;

	/** Checks if the shooter is on the ground */
	virtual bool IsOnGround();

	/** If the current movement base is dynamic, adjust position if it moves*/
	virtual void HandleBaseMovement() const;

	/** Short hand pointer to the container class with all movement related variables*/
	UShooterMovementComponent* Movement;

	/** Short hand pointer to the container struct with all the input states*/
	FShooterInput* Input;

	/** Short hand pointer to the shooter's collider*/
	UCapsuleComponent* Collider;
};
