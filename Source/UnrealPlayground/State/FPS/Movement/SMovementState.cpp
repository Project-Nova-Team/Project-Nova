//Copyright 2021 Brendan Lienau. All Rights Reserved.

#include "SMovementState.h"
#include "../../../Weapon/ShooterCombatComponent.h"

void USMovementState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);

	Movement = Cast<UShooterMovementComponent>(Shooter->GetMovementComponent());
	Input = Shooter->GetInput();
	Collider = Shooter->GetCollider();

	ExpectedHalfHeight = Movement->StandingHeight;
}

void USMovementState::OnEnter()
{
	FString StateName = GetClass()->GetFName().GetPlainNameString();
	StateName.RemoveFromStart("S");
	StateName.RemoveFromEnd("State");
	Movement->State = StateName;
}

void USMovementState::Tick(const float DeltaTime)
{
	HandleBaseMovement();
	Movement->bIsOnGround = IsOnGround();
	RotateCameraFromInput(DeltaTime);
	Movement->CameraRelativeInput = ConvertInputRelativeToCamera();
	CalculateVelocity(DeltaTime);
	SetNewLocation(DeltaTime);
}

FVector USMovementState::ConvertInputRelativeToCamera() const
{
	FVector CameraForward = Shooter->GetCamera()->GetForwardVector();
	FVector CameraRight = Shooter->GetCamera()->GetRightVector();
	FVector RelativeInput = (CameraForward * Input->MoveY) + (CameraRight * Input->MoveX);
	RelativeInput.Z = 0.f;
	RelativeInput.Normalize();

	return RelativeInput;
}

void USMovementState::CheckForJump() const
{
	if (Input->bIsTryingToJump && Movement->bIsOnGround)
	{
		Movement->Velocity.Z += Movement->JumpForce;

		//Lift slighly in the air so the next ground check returns false
		Movement->bIsOnGround = false;
		const FVector CurrentLoc = Collider->GetComponentLocation();
		const FVector Adjustment = FVector(0, 0, FLOOR_DIST);
		Collider->SetWorldLocation(CurrentLoc + Adjustment, true);
	}
}

void USMovementState::CalculateVelocity(const float DeltaTime) const
{
	//If we're airborne we want our input to be scaled by the air control factor
	float AirborneInputFactor = Movement->bIsOnGround ? 1.f : Movement->AirControlMultiplier;

	//This is the vector the shooter is trying to move into from input
	Movement->InputVelocity = Movement->CameraRelativeInput * MoveAcceleration;
	const bool bNoInputAcceleration = Movement->InputVelocity.IsNearlyZero();

	const float CurrentMaxInputVelocity = MaxAppliedInputSpeed * Movement->CameraRelativeInput.Size();

	//If we are airborne, apply a force of gravity
	if (!Movement->bIsOnGround)
	{
		const float GravityScale = FORCE_OF_GRAVITY * Movement->GravityMultiplier;
		const FVector GravityVelocity = FVector::DownVector * GravityScale * DeltaTime;
		Movement->Velocity += GravityVelocity;

		//We are on some kind of sloped surface, apply friction and stop input from applying any velocity
		if (Movement->GroundHitData.IsValidBlockingHit())
		{
			AirborneInputFactor = 0.f;
			//TOOD apply by friction material
			const float FrictionScale =
				FORCE_OF_GRAVITY *
				Movement->FrictionMultiplier *
				0.7f;
			ApplyFrictionToVelocity(Movement->Velocity, 0.f, FrictionScale, DeltaTime);
		}
	}

	//We are on ground so there is no vertical velocity. We may need to apply friction
	else
	{	
		//TODO use the normal of the ground to convert this speed into a sliding force
		Movement->Velocity.Z = 0;
		const float VelocitySizeSquare2D = Movement->Velocity.SizeSquared2D();
		
		//No friction if no velocity
		if (VelocitySizeSquare2D > KINDA_SMALL_NUMBER)
		{	
			//The force of friction is defined as the the force of gravity (-9.81 m/s^2 * mass (we simulate 1)) times the coefficent of friction
			//TODO - Get physics material friction value to work
			const float FrictionScale =
				FORCE_OF_GRAVITY *
				Movement->FrictionMultiplier *
				0.7f;

			const float MaxSquare = CurrentMaxInputVelocity * CurrentMaxInputVelocity;

			//If we are moving faster than possible than analog input currently allows, apply friction
			//TODO - Some non-input-related forces may be desired later. Check if one is applied before applying friction here! 
			if (VelocitySizeSquare2D > MaxSquare * INPUT_EPSILON)
			{
				ApplyFrictionToVelocity(Movement->Velocity, CurrentMaxInputVelocity, FrictionScale, DeltaTime);
			}

			//Friction is also applied when changing direction
			else if (!bNoInputAcceleration)
			{
				const float VelocitySize = FMath::Sqrt(VelocitySizeSquare2D);
				//TODO multiply by ground physics material
				const float FrictionFactor = FMath::Min(DeltaTime * Movement->FrictionMultiplier * Movement->FrictionTurnMultiplier, 1.f);

				//Slowly shift velocity towards the input vector
				const FVector AccelerationDelta = Movement->Velocity - Movement->CameraRelativeInput * VelocitySize;
				Movement->Velocity = Movement->Velocity - (AccelerationDelta * FrictionFactor);
			}
		}		
	}

	//Apply the input velocity to the movement velocity
	if (!bNoInputAcceleration)
	{
		//If we are moving faster than input allows us to via some force, we don't want to kill that force
		const float VelocitySize2D = Movement->Velocity.Size2D();
		const float MaxSpeed = VelocitySize2D > (CurrentMaxInputVelocity * INPUT_EPSILON) ? VelocitySize2D : MaxAppliedInputSpeed;
		Movement->Velocity += Movement->InputVelocity * AirborneInputFactor * DeltaTime;
		Movement->Velocity = Movement->Velocity.GetClampedToMaxSize2D(MaxSpeed);
	}
	
	//Clamp the final velocity in the event we fall for a very long time or are launched at a ridiculous speed
	const FVector CurrentVelocity = Movement->Velocity;
	const FVector XYVelocity = FVector(CurrentVelocity.X, CurrentVelocity.Y, 0).GetClampedToMaxSize(Movement->MaxHorizontalSpeed);
	const float ZVelocity = FMath::Clamp(CurrentVelocity.Z, -Movement->MaxVerticalSpeed, Movement->MaxVerticalSpeed);

	Movement->Velocity = FVector(XYVelocity.X, XYVelocity.Y, ZVelocity);
}

void USMovementState::ApplyFrictionToVelocity(FVector& InVelocity, const float MinPostFrictionVelocity, const float Friction, const float DeltaTime) const 
{
	FVector Velocity2D = FVector(InVelocity.X, InVelocity.Y, 0);
	const float MinSizeSquare = MinPostFrictionVelocity * MinPostFrictionVelocity;
	const FVector VelocityNormal = Velocity2D.GetSafeNormal2D();
	FVector FrictionForce = -VelocityNormal;

	FrictionForce *= Friction * DeltaTime;

	FVector AfterFrictionVelocity = Velocity2D + FrictionForce;
	//Create a normal vector that points away from the max input size scaled velocity
	const FVector AfterFrictionDirection = (AfterFrictionVelocity - (VelocityNormal * MinSizeSquare)).GetSafeNormal2D();

	//If the dot product of these normals are below or close to zero, friction has changed the velocity below the max size
	if ((VelocityNormal | AfterFrictionDirection) < KINDA_SMALL_NUMBER)
	{
		AfterFrictionVelocity = VelocityNormal * MinPostFrictionVelocity;
	}

	InVelocity = FVector(AfterFrictionVelocity.X, AfterFrictionVelocity.Y, InVelocity.Z);
}

void USMovementState::RotateCameraFromInput(const float DeltaTime)
{
	const float RecoilVelocity = Shooter->GetCombat()->GetWeaponRecoilVelocity();
	const bool bRecoilActive = RecoilVelocity > 0.f;

	FRotator AnchorRotation = Shooter->GetAnchor()->GetComponentRotation();
	AnchorRotation.Yaw += (Input->LookX * Movement->CameraSensitivity * DeltaTime);
	AnchorRotation.Pitch = FMath::Clamp(
		AnchorRotation.Pitch + (Input->LookY * Movement->CameraSensitivity * DeltaTime),
		Movement->CameraMinAngle,
		Movement->CameraMaxAngle);

	Shooter->GetAnchor()->SetWorldRotation(AnchorRotation);
	const FRotator RelativeLook = Shooter->GetCamera()->GetRelativeRotation();

	//Weapon was fired, apply recoil impulse
	if (RecoilVelocity > 0.f)
	{	
		float NewPitch = RelativeLook.Pitch + RecoilVelocity * DeltaTime;
		const float AngularLimit = Shooter->GetCombat()->GetPrimaryWeapon()->GetRecoilLimit();

		//The new recoil pitch would have us look further upwards than we allow, clamp it
		if (NewPitch + AnchorRotation.Pitch > Movement->CameraMaxAngle)
		{
			NewPitch = Movement->CameraMaxAngle - AnchorRotation.Pitch;
		}

		//Recoil has pushed us past the maximum vertical effect, clamp it
		//TODO start applying recoil along the XY axis instead?
		else if (NewPitch > AngularLimit)
		{
			NewPitch = AngularLimit;
		}

		const FRotator CameraRotation = FRotator(NewPitch, RelativeLook.Yaw, RelativeLook.Roll);
		Shooter->GetCamera()->SetRelativeRotation(CameraRotation);
	}

	//Weapon impulse ended, reset towards the anchors forward
	else if (RelativeLook.Pitch > 0.f)
	{
		const float Delta = Shooter->GetCombat()->GetPrimaryWeapon()->GetRecoilRecovery() * DeltaTime;
		float NewPitch = RelativeLook.Pitch - Delta;

		//Don't rotate further down than where we started
		NewPitch = NewPitch < 0.f ? 0 : NewPitch;

		const FRotator CameraRotation = FRotator(NewPitch, RelativeLook.Yaw, RelativeLook.Roll);
		Shooter->GetCamera()->SetRelativeRotation(CameraRotation);
	}
}

void USMovementState::SetNewLocation(const float DeltaTime) const
{
	//No velocity, no movement
	if (Movement->Velocity.IsNearlyZero())
	{
		Input->bIsMoving = false;
		return;
	}

	Input->bIsMoving = true;
	FHitResult CollisionHit(1.f);
	
	const FVector Delta = Movement->Velocity * DeltaTime;
	const FVector DeltaMovement = Movement->bIsOnGround ? CalculateSurfaceMovement(Delta, Movement->GroundHitData) : Delta;
	MoveShooter(DeltaMovement, CollisionHit);

	//We hit an obstruction, either step up or reduce velocity
	if (CollisionHit.IsValidBlockingHit())
	{
		CorrectCollision(DeltaTime, Delta, CollisionHit);
	}
}

void USMovementState::MoveShooter(const FVector DeltaMovement, FHitResult& OutCollisionResult, const bool bAdjustZ /*=true*/) const
{
	FVector CurrentLocation = Collider->GetComponentLocation();

	if (bAdjustZ && Movement->bIsOnGround)
	{
		const float HalfHeight = Collider->GetScaledCapsuleHalfHeight();
		CurrentLocation.Z = Movement->GroundHitData.ImpactPoint.Z + HalfHeight + FLOOR_DIST;
		Collider->SetWorldLocation(CurrentLocation);
	}

	Collider->SetWorldLocation(CurrentLocation + DeltaMovement, true, &OutCollisionResult);
}

FVector USMovementState::CalculateSurfaceMovement(FVector DeltaMovement, const FHitResult SurfaceHit) const
{
	FVector NewDelta = FVector(DeltaMovement.X, DeltaMovement.Y, 0);

	if (DeltaMovement.IsNearlyZero())
	{
		return FVector::ZeroVector;
	}

	if (SurfaceHit.IsValidBlockingHit())
	{
		//We are on a sloped surface, adjust the movement vector based on the slope incline
		if (SurfaceHit.ImpactNormal.Z < 1 - KINDA_SMALL_NUMBER && SurfaceHit.ImpactNormal.Z  > KINDA_SMALL_NUMBER)
		{			
			const float DeltaDot = (NewDelta | SurfaceHit.ImpactNormal);
			NewDelta = FVector(NewDelta.X, NewDelta.Y, (-DeltaDot / SurfaceHit.ImpactNormal.Z));
		}
	}
	
	return NewDelta;
}

void USMovementState::CorrectCollision(const float DeltaTime, FVector Delta, FHitResult& CollisionHit) const
{
	const FVector VelocityNormal = Movement->Velocity.GetSafeNormal();

	//Hit the floor of another walkable ramp
	if (Movement->bIsOnGround && CollisionHit.ImpactNormal.Z >= Movement->MaxSlopeZ)
	{
		NewInclineAdjust(Delta, CollisionHit);
	}

	//Hit the floor of an unwalkable surface
	else if(Movement->GroundHitData.IsValidBlockingHit() && !Movement->bIsOnGround)
	{
		//Convert downwards gravity speed into movement that runs down the slope
		FVector Projection = FVector::VectorPlaneProject(VelocityNormal, Movement->GroundHitData.ImpactNormal).GetSafeNormal();
		Projection.Z = -FMath::Abs(Projection.Z);
		const float GravityStrength = FMath::Abs(Movement->Velocity.Z);
		const FVector SlidingDelta = GravityStrength * DeltaTime * Projection;
				
		const FVector CollisionNormal2D = Movement->GroundHitData.ImpactNormal.GetSafeNormal2D();
		const FVector VelocityNormal2D = VelocityNormal.GetSafeNormal2D();

		Delta = SlidingDelta;
		//We are moving into the slope, convert velocity to run tangent along it
		if ((CollisionNormal2D | VelocityNormal2D) < 0.f)
		{
			CorrectVelocityAlongCollision(Movement->Velocity, CollisionNormal2D);
			Delta += Movement->Velocity * DeltaTime;
		}

		MoveShooter(Delta, CollisionHit);
		
		//We reached the edge of the unwalkable slope and have need to slide down it a little bit
		if (CollisionHit.IsValidBlockingHit())
		{
			NewInclineAdjust(Delta, CollisionHit);
		}
	}

	//These first two checks resolved the collision, break out
	if (!CollisionHit.IsValidBlockingHit())
	{
		return;
	}

	const float CurrentZ = Collider->GetComponentLocation().Z;

	//Hit a wall above the base of the collider, adjust velocity as if we hit a wall
	if (CollisionHit.ImpactPoint.Z > CurrentZ - Collider->GetScaledCapsuleHalfHeight_WithoutHemisphere())
	{
		//Top of the collider was hit by something
		if (CollisionHit.ImpactNormal.Z < -KINDA_SMALL_NUMBER && Movement->Velocity.Z > 0)
		{
			Movement->Velocity.Z = 0;
		}

		CorrectVelocityAlongCollision(Movement->Velocity, CollisionHit.ImpactNormal);

		//Attempt the move again with the new velocity
		const FVector NewDelta = Movement->Velocity * DeltaTime;
		FVector NewDeltaMovement = CalculateSurfaceMovement(NewDelta, Movement->GroundHitData);
		MoveShooter(NewDeltaMovement, CollisionHit);
	}

	//Hit a barricade of some kind, either step up or slow down
	else if(CollisionHit.ImpactNormal.Z <= KINDA_SMALL_NUMBER && CollisionHit.ImpactNormal.Z >= -KINDA_SMALL_NUMBER)
	{		
		FVector StepPosition;

		if (CanStepOntoSurface(CollisionHit, StepPosition))
		{
			//CanStepOntoSurface already performs a sweep test so we don't have to worry about sweeping the movement
			Collider->SetWorldLocation(StepPosition);

			const float RemainingMovePercent = 1.f - CollisionHit.Time;
			const FVector NewDelta = Movement->Velocity * DeltaTime * RemainingMovePercent;
			FVector NewDeltaMovement = CalculateSurfaceMovement(NewDelta, Movement->GroundHitData);
			MoveShooter(NewDeltaMovement, CollisionHit, false);
		}

		//We can't step on this surface so slow velocity down as if it impacted a wall
		else
		{
			CorrectVelocityAlongCollision(Movement->Velocity, CollisionHit.ImpactNormal);

			//Attempt the move again with the new velocity
			const FVector NewDelta = Movement->Velocity * DeltaTime;
			FVector NewDeltaMovement = CalculateSurfaceMovement(NewDelta, Movement->GroundHitData);
			MoveShooter(NewDeltaMovement, CollisionHit);
		}
	}
}

void USMovementState::NewInclineAdjust(FVector Delta, FHitResult& CollisionHit) const
{
	const FHitResult NewInclineHit = CollisionHit;

	//Instead move just before where the last moved failed
	const float MovePercent = CollisionHit.Time - KINDA_SMALL_NUMBER;
	FVector DeltaMovement = Delta * MovePercent;
	MoveShooter(DeltaMovement, CollisionHit);

	//Then move the remaining distance along the new incline normal
	const float RemainingMovePercent = 1.f - MovePercent;
	DeltaMovement = CalculateSurfaceMovement(Delta * RemainingMovePercent, NewInclineHit);
	MoveShooter(DeltaMovement, CollisionHit);
}

void USMovementState::CorrectVelocityAlongCollision(FVector& InOutVelocity, const FVector CollisionNormal) const
{
	//Reduce velocity by projecting the current velocity onto the collision normal		
	const FVector CollisionNormal2D = CollisionNormal.GetSafeNormal2D();
	const FVector VelocityNormal = InOutVelocity.GetSafeNormal2D();
	float Dot = 1 + (VelocityNormal | CollisionNormal2D);
	FVector Projection = FVector::VectorPlaneProject(VelocityNormal, CollisionNormal2D);
	Projection.Z = 0;
	Projection.Normalize();

	InOutVelocity = Projection * Dot * Movement->Velocity.Size();

	//Push us away from the wall ever so slightly
	const FVector CurrentLocation = Collider->GetComponentLocation();
	Collider->SetWorldLocation(CurrentLocation + (CollisionNormal2D * KINDA_SMALL_NUMBER));
}

bool USMovementState::CanStepOntoSurface(const FHitResult CollisionData, FVector& OutStepPosition) const
{	
	const FVector CurrentLocation = Collider->GetComponentLocation();
	const float HalfHeight = Collider->GetScaledCapsuleHalfHeight();
	const FVector PushAmount = Movement->Velocity.GetSafeNormal2D(); //TODO magic number

	FVector TraceEnd = CollisionData.ImpactPoint + PushAmount;
	TraceEnd.Z = CurrentLocation.Z - HalfHeight;
	TraceEnd.Z -= Movement->bIsOnGround ? FLOOR_DIST : 0.f; //If we are grounded remove the variable amount we hover off the floor
	
	const FVector TraceStart = TraceEnd + FVector(0, 0, Movement->MaxStepHeight);
	
	FHitResult StepHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Shooter);

	const bool bHit = Shooter->GetWorld()->LineTraceSingleByChannel(StepHit, TraceStart, TraceEnd, ECC_Pawn, QueryParams);

#if WITH_EDITOR
	if(Shooter->bTraceDebug)
		DrawDebugLine(Shooter->GetWorld(), TraceStart, TraceEnd, FColor::Purple, true);
#endif

	//If a line trace finds a valid surface, sweep to see if the shooter can stand there
	if (StepHit.IsValidBlockingHit() && StepHit.ImpactNormal.Z >= Movement->MaxSlopeZ)
	{
		const float NewZ = StepHit.ImpactPoint.Z + HalfHeight + FLOOR_DIST;
		const float Radius = Collider->GetScaledCapsuleRadius();
		const FCollisionShape Capsule = FCollisionShape::MakeCapsule(Radius, HalfHeight);
		
		const FVector SweepStart = FVector(CurrentLocation.X, CurrentLocation.Y, NewZ);
		const FVector SweepEnd = SweepStart + PushAmount;

		const bool bStepHit = Shooter->GetWorld()->SweepSingleByChannel(StepHit, SweepStart, SweepEnd, FQuat::Identity, ECC_Pawn, Capsule, QueryParams);
		
#if WITH_EDITOR
		if (Shooter->bTraceDebug)
		{
			DrawDebugCapsule(Shooter->GetWorld(), SweepStart, HalfHeight, Radius, FQuat::Identity, FColor::Purple, false, 2.f);
			DrawDebugCapsule(Shooter->GetWorld(), SweepEnd, HalfHeight, Radius, FQuat::Identity, FColor::Purple, false, 2.f);
		}
#endif

		if (!bStepHit)
		{
			OutStepPosition = SweepEnd;
			return true;
		}
	}

	return false;
}

bool USMovementState::IsOnGround()
{
	float Radius, HalfHeight;
	Collider->GetScaledCapsuleSize(Radius, HalfHeight);
	const FVector CapsuleLocation = Collider->GetComponentLocation();

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Shooter);

	//Create a box bounded inside the capsule
	//.707 ~= fast sqrt(2)/2
	const float TestBoxWidth = Radius * .707f * GROUND_CHECK_EPSILON;
	const float TestBoxHeight = ((HalfHeight - Radius) + 1.f) * GROUND_CHECK_EPSILON;

	//We need to sweep a bit deeper into the floor than we are hovering above the surface
	//Not sure why such a high tolerance (20%!) is requried to make this work
	const float FloorCorrection = (1.2f * FLOOR_DIST) / GROUND_CHECK_EPSILON;
	FCollisionShape TestBox = FCollisionShape::MakeBox(FVector(TestBoxWidth, TestBoxWidth, TestBoxHeight));
	FVector SweepEnd = CapsuleLocation - FVector(0.f, 0.f, Radius + FloorCorrection);

	//Sweep against pawn layer
	bool bHit = Shooter->GetWorld()->SweepSingleByChannel(Hit, CapsuleLocation, SweepEnd, FQuat::Identity, ECC_Pawn, TestBox, QueryParams);

	//If we miss, try again after rotating the sweep shape 45 degrees
	//Note this approach is not perfect and can still fail in some cases
	if (!bHit)
	{
		bHit = Shooter->GetWorld()->SweepSingleByChannel(Hit, CapsuleLocation, SweepEnd, FQuat(FVector(0.f, 0.f, 1.f), PI * .25f), ECC_Pawn, TestBox, QueryParams);
	}

#if WITH_EDITOR
	if (Shooter->bTraceDebug)
	{
		DrawDebugBox(Shooter->GetWorld(), CapsuleLocation, TestBox.GetExtent(), FQuat::Identity, FColor::Red, false, 0.016f);
		DrawDebugBox(Shooter->GetWorld(), SweepEnd, TestBox.GetExtent(), FQuat::Identity, FColor::Red, false, 0.016f);
	}
#endif

	Movement->GroundHitData = Hit;

	if (Hit.IsValidBlockingHit())
	{
		Movement->Base.BaseObject = Hit.GetComponent();
		Movement->Base.bIsDynamic = Movement->Base.BaseObject->Mobility == EComponentMobility::Movable;

		if (Movement->Base.bIsDynamic)
		{
			Movement->Base.OldLocation = Movement->Base.BaseObject->GetComponentLocation();
		}
	}
	//If the surface isn't too steep and the sweep hit something we are on ground
	return (bHit && Hit.ImpactNormal.Z >= Movement->MaxSlopeZ);
}

void USMovementState::HandleBaseMovement() const
{
	if (Movement->bIsOnGround && Movement->Base.bIsDynamic)
	{
		const FVector Delta = Movement->Base.BaseObject->GetComponentLocation() - Movement->Base.OldLocation;
		
		if (!Delta.IsNearlyZero())
		{
			FHitResult Collision;
			MoveShooter(Delta, Collision);
			
			///TODO check for hits and move based on time
		}
	}
}