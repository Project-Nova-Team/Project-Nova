#include "SLookState.h"

void USLookState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);

	Movement = Cast<UShooterMovementComponent>(Shooter->GetMovementComponent());
	Input = Shooter->GetInput();
}

void USLookState::OnEnter()
{
	Super::OnEnter();

	APlayerController* ShooterController = Cast<APlayerController>(Shooter->GetController());

	if (ShooterController != nullptr)
	{
		ShooterController->PlayerCameraManager->StartCameraFade(1, 0, 6, FLinearColor(FColor::Black));
	}
}

void USLookState::RotateCameraFromInput(const float DeltaTime)
{
	FRotator AnchorRotation = Shooter->GetAnchor()->GetRelativeRotation();

	AnchorRotation.Yaw = FMath::Clamp(
		AnchorRotation.Yaw + (Input->LookX * Movement->CameraSensitivity * DeltaTime),
		-70.f,
		70.f);
	AnchorRotation.Pitch = FMath::Clamp(
		AnchorRotation.Pitch + (Input->LookY * Movement->CameraSensitivity * DeltaTime),
		Movement->CameraMinAngle * 0.8f,
		Movement->CameraMaxAngle * 0.8f);

	Shooter->GetAnchor()->SetRelativeRotation(AnchorRotation);
}

void USLookState::Tick(const float DeltaTime)
{
	RotateCameraFromInput(DeltaTime);
}