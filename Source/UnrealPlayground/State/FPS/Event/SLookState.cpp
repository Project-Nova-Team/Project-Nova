#include "SLookState.h"

void USLookState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);

	Movement = Cast<UShooterMovementComponent>(Shooter->GetMovementComponent());
	Input = Shooter->GetInput();

	Cast<APlayerController>(Shooter->GetController())->PlayerCameraManager->StartCameraFade(2, 0, 6, FLinearColor(FColor::Black));
}

void USLookState::RotateCameraFromInput(const float DeltaTime)
{
	FRotator AnchorRotation = Shooter->GetAnchor()->GetComponentRotation();
	AnchorRotation.Yaw += (Input->LookX * Movement->CameraSensitivity * DeltaTime);
	AnchorRotation.Pitch = FMath::Clamp(
		AnchorRotation.Pitch + (Input->LookY * Movement->CameraSensitivity * DeltaTime),
		Movement->CameraMinAngle,
		Movement->CameraMaxAngle);

	Shooter->GetAnchor()->SetWorldRotation(AnchorRotation);
}

void USLookState::Tick(const float DeltaTime)
{
	RotateCameraFromInput(DeltaTime);
}