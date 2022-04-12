#include "ShooterCutscene.h"
#include "Camera/CameraComponent.h"
#include "../Player/Shooter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/PlayerController.h"

AShooterCutscene::AShooterCutscene()
{
	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	SetRootComponent(Body);
	Body->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(Body);
	Arms->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Body, TEXT("C_Head_jnt"));
	Camera->SetRelativeScale3D(FVector(.25f));

	Arms->SetVisibility(false);
	Body->SetVisibility(false);
}

void AShooterCutscene::PlayCutscene(UAnimMontage* Animation)
{
	Shooter->SetStateOverride("Cutscene");

	SetActorLocation(Shooter->GetBodyMesh()->GetComponentLocation());
	SetActorRotation(Shooter->GetActorRotation());

	Shooter->GetArmsMesh()->SetVisibility(false);
	Shooter->GetBodyMesh()->SetVisibility(false);
	Arms->SetVisibility(true);
	Body->SetVisibility(true);

	Arms->GetAnimInstance()->Montage_Play(Animation);
	Body->GetAnimInstance()->Montage_Play(Animation);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterCutscene::FinishCutscene, BlendTime);
	Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(this, BlendTime, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AShooterCutscene::FinishCutscene()
{
	Arms->SetVisibility(false);
	Body->SetVisibility(false);
	Shooter->GetArmsMesh()->SetVisibility(true);
	Shooter->GetBodyMesh()->SetVisibility(true);

	//Moves the shooter into the location/rotation the cutscene ended in 
	const FName RootBone = TEXT("C_Root_jnt");
	Shooter->SetActorLocation(Body->GetBoneLocation(RootBone) - FVector(0.f, 0.f, Shooter->GetBodyMesh()->GetRelativeLocation().Z));
	Shooter->SetActorRotation(FRotator(0.f, Body->GetBoneQuaternion(RootBone).Rotator().Yaw, 0.f));

	//Make the shooter look in the same direction the cutscene ended in, but without roll or yaw
	const float HeadPitch = Body->GetBoneQuaternion(TEXT("C_Head_jnt")).Rotator().Pitch;
	Shooter->GetAnchor()->SetRelativeRotation(FRotator(HeadPitch, 0.f, 0.f));

	//Give player control again
	Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(Shooter, BlendTime, EViewTargetBlendFunction::VTBlend_Cubic);
	Shooter->SetStateOverride("Walking"); //this should maybe be done once the blend finishes
}
