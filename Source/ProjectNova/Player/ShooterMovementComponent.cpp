#include "ShooterMovementComponent.h"

UShooterMovementComponent::UShooterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	CameraSensitivity = 200.f;
	CameraMaxAngle = 89.f;
	CameraMinAngle = -89.f;
	CameraHeight = 60.f;
	InteractionDistance = 300.f;

	MaxSurfaceAngle = 45.f;
	MaxStepHeight = 20.f;
	StandingHeight = 90.f;
	CollisionRadius = 30.f;

	FrictionMultiplier = 5.f;
	FrictionTurnMultiplier = 10.f;
	WallFrictionMultiplier = -0.25f;
	GravityMultiplier = 2.2f;

	AirControlMultiplier = 0.f;
	JumpForce = 900.f;
	MaxHorizontalSpeed = 5000.f;
	MaxVerticalSpeed = 20000.f;

	WalkAcceleration = 2000.f;
	WalkMaxSpeed = 500.f;

	RunAcceleration = 3200.f;
	RunMaxSpeed = 800.f;
	RunMinSpeed = 490.f;
	RunLookMoveMinDot = 0.5f;
	
	CrouchAcceleration = 1000.f;
	CrouchMaxSpeed = 250.f;
	CrouchTime = 1.f;

	VentSpeed = 5.f;
	VentTurnSpeed = 15.f;
	VentCameraLocation = FVector(-6.75f, 4.565f, -66.11f);
}

void UShooterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	MaxSlopeZ = FMath::Acos(FMath::DegreesToRadians(MaxSurfaceAngle));
	bIsOnGround = true;
}
