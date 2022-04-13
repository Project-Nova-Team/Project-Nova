#include "ShooterCutscene.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "../Player/Shooter.h"
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

void AShooterCutscene::PlayCutscene(UAnimMontage* Animation, const FString& ExitState, const FTransform& StartingTransform)
{
	Shooter->SetStateOverride("Cutscene");

	SetActorLocation(Shooter->GetBodyMesh()->GetComponentLocation());
	SetActorRotation(Shooter->GetActorRotation());
	
	//Sloppy check
	if (StartingTransform.GetLocation() != FVector::ZeroVector)
	{
		Timer = 0;
		EntryTransform.SetLocation(GetActorLocation());
		EntryTransform.SetRotation(GetActorRotation().Quaternion());
		BlendTowardsTransform(StartingTransform);
	}

	Shooter->GetArmsMesh()->SetVisibility(false);
	Shooter->GetBodyMesh()->SetVisibility(false);
	Arms->SetVisibility(true);
	Body->SetVisibility(true);

	Arms->GetAnimInstance()->Montage_Play(Animation);
	Body->GetAnimInstance()->Montage_Play(Animation);

	FTimerDelegate FinishDelegate;
	FinishDelegate.BindUObject(this, &AShooterCutscene::FinishCutscene, ExitState);
	GetWorldTimerManager().SetTimer(TimerHandle, FinishDelegate, BlendTime, false);

	Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(this, BlendTime, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AShooterCutscene::FinishCutscene(const FString ExitState)
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

	//Move camera back into position
	Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(Shooter, BlendTime, EViewTargetBlendFunction::VTBlend_Cubic);

	//Put the player in the desired state once the cutscene finishes
	FTimerDelegate TimerCallback;
	TimerCallback.BindUObject(this, &AShooterCutscene::BlendComplete, ExitState);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, BlendTime, false);
}

void AShooterCutscene::BlendComplete(const FString ExitState)
{
	Shooter->SetStateOverride(ExitState);
}

void AShooterCutscene::BlendTowardsTransform(FTransform Transform)
{
	Timer += GetWorld()->GetDeltaSeconds();
	const float Alpha = FMath::Clamp(Timer / BlendTime, 0.f, 1.f);

	SetActorLocation(FMath::Lerp(EntryTransform.GetLocation(), Transform.GetLocation(), Alpha));
	SetActorRotation(FMath::Lerp(EntryTransform.GetRotation(), Transform.GetRotation(), Alpha));

	if (Alpha < 1.f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &AShooterCutscene::BlendTowardsTransform, Transform);
		GetWorldTimerManager().SetTimerForNextTick(Delegate);
	}
}
