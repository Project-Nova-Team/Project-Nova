#include "SDeathState.h"
#include "Camera/PlayerCameraManager.h"
#include "../../../Animation/ShooterAnimInstance.h"

void USDeathState::OnEnter()
{
	//Just play a death animation
	APlayerCameraManager* const CameraManager = Cast<APlayerController>(Shooter->GetController())->PlayerCameraManager;
	UShooterAnimInstance* const AnimationInstance = Cast<UShooterAnimInstance>(Shooter->GetSkeletalMeshComponent()->GetAnimInstance());
	UCameraAnim* const Animation = AnimationInstance->CameraDeathAnimation;

	CameraManager->PlayCameraAnim(Animation);
}
