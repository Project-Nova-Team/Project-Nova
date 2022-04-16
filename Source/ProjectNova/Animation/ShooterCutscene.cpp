#include "ShooterCutscene.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "../Player/Shooter.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

AShooterCutscene::AShooterCutscene()
{
	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	SetRootComponent(Body);

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(Body);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Body, TEXT("C_Head_jnt"));
	Camera->SetRelativeScale3D(FVector(.25f));

	Arms->SetVisibility(false);
	Body->SetVisibility(false);
	Body->bNoSkeletonUpdate = true;
	Arms->bNoSkeletonUpdate = true;
}

void AShooterCutscene::BeginPlay()
{
	Super::BeginPlay();

	Body->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &AShooterCutscene::FinishCutscene);
}

void AShooterCutscene::PlayCutscene(UAnimMontage* Animation, const FString FinishState, const FTransform& StartingTransform)
{
	Body->bNoSkeletonUpdate = false;
	Arms->bNoSkeletonUpdate = false;
	Shooter->GetBodyMesh()->bNoSkeletonUpdate = true;
	Shooter->GetArmsMesh()->bNoSkeletonUpdate = true;

	Shooter->SetStateOverride("Cutscene");
	ExitState = FinishState;

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

	Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(this, BlendTime, EViewTargetBlendFunction::VTBlend_Linear);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterCutscene::ReenableSkeleton, BlendTime);
}

void AShooterCutscene::ReenableSkeleton()
{
	Shooter->GetBodyMesh()->bNoSkeletonUpdate = false;
	Shooter->GetArmsMesh()->bNoSkeletonUpdate = false;
}

void AShooterCutscene::FinishCutscene(UAnimMontage* Animation, bool bInterrupted)
{
	Arms->SetVisibility(false);
	Body->SetVisibility(false);
	Shooter->GetArmsMesh()->SetVisibility(true);
	Shooter->GetBodyMesh()->SetVisibility(true);

	Body->bNoSkeletonUpdate = true;
	Arms->bNoSkeletonUpdate = true;
	
	//Moves the shooter into the location the cutscene ended in 
	const FName RootBone = TEXT("C_Root_jnt");
	Shooter->SetActorLocation(Body->GetBoneLocation(RootBone) - FVector(0.f, 0.f, Shooter->GetBodyMesh()->GetRelativeLocation().Z));

	//Make the shooter look in the same direction the cutscene ended in
	const FRotator LookRotation = Camera->GetComponentRotation();
	Shooter->SetActorRotation(FRotator(0.f, LookRotation.Yaw, 0.f));
	Shooter->GetAnchor()->SetRelativeRotation(FRotator(LookRotation.Pitch, 0.f, 0.f));

	//Move camera back into position
	Shooter->SetStateOverride(ExitState);

	if (Shooter->GetController<APlayerController>())
	{
		Shooter->GetController<APlayerController>()->SetViewTargetWithBlend(Shooter, BlendTime, EViewTargetBlendFunction::VTBlend_Linear);
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
