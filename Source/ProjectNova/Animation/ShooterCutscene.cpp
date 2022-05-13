#include "ShooterCutscene.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "../Player/Shooter.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "../ShooterHUD.h"
#include "../ShooterController.h"

AShooterCutscene::AShooterCutscene()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	SetRootComponent(Mesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Mesh, TEXT("C_Head_jnt"));
	Camera->SetRelativeScale3D(FVector(.25f));

	Mesh->SetVisibility(false);
	Mesh->bNoSkeletonUpdate = true;
}

void AShooterCutscene::PlayCutscene(UAnimMontage* Animation, const FString FinishState, const FTransform& StartingTransform)
{
	StartCinematic(StartingTransform);
	float AnimTime = Mesh->GetAnimInstance()->Montage_Play(Animation);

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &AShooterCutscene::EndCinematic, FinishState);
	GetWorldTimerManager().SetTimer(TimerHandle, Delegate, AnimTime - Animation->BlendOut.GetBlendTime(), false);
}

void AShooterCutscene::StartCinematic(const FTransform& StartingTransform)
{
	Mesh->bNoSkeletonUpdate = false;
	Shooter->GetBodyMesh()->bNoSkeletonUpdate = true;
	Shooter->GetArmsMesh()->bNoSkeletonUpdate = true;

	Shooter->SetStateOverride("Cutscene");

	Shooter->GetArmsMesh()->SetVisibility(false);
	Shooter->GetBodyMesh()->SetVisibility(false);
	Shooter->GetWeaponArmsMesh()->SetVisibility(false);
	Mesh->SetVisibility(true);

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

	if (AShooterController* Controller = Shooter->GetController<AShooterController>())
	{
		Controller->ShooterHUD->SetWeaponDisplayVisibility(false);
		Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(this, BlendTime, EViewTargetBlendFunction::VTBlend_Linear);
	}

	GetWorldTimerManager().SetTimer(SkeletalHandle, this, &AShooterCutscene::ReenableSkeleton, BlendTime);
}

void AShooterCutscene::EndCinematic(const FString FinishState)
{
	ExitState = FinishState;
	FinishCutscene();
}

void AShooterCutscene::PlayAnimation(UAnimMontage* Animation)
{
	Mesh->GetAnimInstance()->Montage_Play(Animation);
}

void AShooterCutscene::ReenableSkeleton()
{
	Shooter->GetBodyMesh()->bNoSkeletonUpdate = false;
	Shooter->GetArmsMesh()->bNoSkeletonUpdate = false;
}

void AShooterCutscene::FinishCutscene()
{
	Mesh->SetVisibility(false);
	
	Shooter->GetBodyMesh()->SetVisibility(true);

	if (Shooter->GetCombat()->GetHeldWeapon())
	{
		Shooter->GetWeaponArmsMesh()->SetVisibility(true);	
	}

	else
	{
		Shooter->GetArmsMesh()->SetVisibility(true);
	}
	

	Mesh->bNoSkeletonUpdate = true;

	//Moves the shooter into the location the cutscene ended in 
	const FName RootBone = TEXT("C_Root_jnt");
	Shooter->SetActorLocation(Mesh->GetBoneLocation(RootBone) - FVector(0.f, 0.f, Shooter->GetBodyMesh()->GetRelativeLocation().Z));

	//Make the shooter look in the same direction the cutscene ended in
	const FRotator LookRotation = Camera->GetComponentRotation();
	Shooter->SetActorRotation(FRotator(0.f, LookRotation.Yaw, 0.f));
	Shooter->GetAnchor()->SetRelativeRotation(FRotator(LookRotation.Pitch, 0.f, 0.f));

	//Move camera back into position
	Shooter->SetStateOverride(ExitState);

	if (AShooterController* Controller = Shooter->GetController<AShooterController>())
	{
		Controller->ShooterHUD->SetWeaponDisplayVisibility(true);
		Controller->SetViewTargetWithBlend(Shooter, BlendTime, EViewTargetBlendFunction::VTBlend_Linear);
	}
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
