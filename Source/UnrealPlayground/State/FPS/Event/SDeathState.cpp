#include "SDeathState.h"
#include "Camera/PlayerCameraManager.h"

void USDeathState::OnEnter()
{
	APlayerCameraManager* CameraManager = Cast<APlayerController>(Shooter->GetController())->PlayerCameraManager;

	//TODO lets hook up an animation in the UShooterAnimInstance.
	//Even though the anim instance is never going to play it, its a better place to live than on the shooter
	//CameraManager->PlayCameraAnim()
}