#include "Shooter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "../Utility/DelayedActionManager.h"
#include "../ShooterGameMode.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Gameplay/HealthComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "../Gameplay/VaultTrigger.h"
#include "../Gameplay/InteractiveObject.h"
#include "../Gameplay/MeleeComponent.h"

//Lazy
#include "../Animation/ShooterAnimInstance.h"

void FShooterInput::Tick(const float DeltaTime)
{
	bIsTryingToCrouch = false;
	bIsTryingToProne = false;

	HandleCrouchInputStates(DeltaTime);
}

void FShooterInput::HandleCrouchInputStates(const float DeltaTime)
{
	if (bIsHoldingCrouch)
	{
		CurrentCrouchHoldTime += DeltaTime;
		if (CurrentCrouchHoldTime >= Owner->ShooterMovement->ProneInputTime)
		{		
			CurrentCrouchHoldTime = 0;
			bIsHoldingCrouch = false;
			bIsTryingToProne = true;
		}
	}

	else
	{
		if (bWasHoldingCrouch)
		{
			bIsTryingToCrouch = true;
			CurrentCrouchHoldTime = 0;
		}
	}

	bWasHoldingCrouch = bIsHoldingCrouch;
}


AShooter::AShooter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	Collider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	CameraAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("Anchor"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ShooterMovement = CreateDefaultSubobject<UShooterMovementComponent>(TEXT("Movement"));
	Melee = CreateDefaultSubobject<UMeleeComponent>(TEXT("Melee"));

	SetRootComponent(Collider);
	CameraAnchor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->AttachToComponent(CameraAnchor, FAttachmentTransformRules::KeepRelativeTransform);
	Melee->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);

	Collider->SetCollisionProfileName("Pawn");
	Collider->SetCapsuleHalfHeight(ShooterMovement->StandingHeight);
	Collider->SetCapsuleRadius(ShooterMovement->CollisionRadius);
	CameraAnchor->SetRelativeLocation(FVector(0, 0, ShooterMovement->CameraHeight));

	ShooterSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	ShooterSkeletalMesh->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(ShooterSkeletalMesh, TEXT("WeaponSocket"));

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetUpConstruction(Camera, WeaponMesh);

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	PerceptionSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus Source"));
}

void AShooter::BeginPlay()
{
	Super::BeginPlay();

	Collider->SetCapsuleHalfHeight(ShooterMovement->StandingHeight);
	Collider->SetCapsuleRadius(ShooterMovement->CollisionRadius);
	CameraAnchor->SetRelativeLocation(FVector(0, 0, ShooterMovement->CameraHeight));

	StateMachine = NewObject<UShooterStateMachine>();
	StateMachine->Initialize(this);
	Cast<UShooterAnimInstance>(ShooterSkeletalMesh->GetAnimInstance())->BindVault(); //laaazy

	InputState.Owner = this;

	OnActorBeginOverlap.AddDynamic(this, &AShooter::OnTriggerEnter);
	OnActorEndOverlap.AddDynamic(this, &AShooter::OnTriggerExit);
	Health->OnDeath.AddDynamic(this, &AShooter::HandleDeath);
}

void AShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	InputState.Tick(DeltaTime);
	StateMachine->Tick(DeltaTime);
	ScanInteractiveObject();

	if (InputState.bIsTryingToThrowPrimary)
	{
		MakeSound(NoiseAmount);
		InputState.bIsTryingToThrowPrimary = false;
	}
}

void AShooter::ScanInteractiveObject()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Camera->GetForwardVector() * FMath::Min(ShooterMovement->StandingHeight * 2.f, ShooterMovement->InteractionDistance);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(ScanHit, TraceStart, TraceEnd, ECC_Camera, QueryParams);

	//We're looking at an object that is interactive
	if(bHit && ScanHit.Actor != nullptr && ScanHit.Actor->Implements<UInteractiveObject>())
	{	
		//Lets us do UI things in blueprint
		OnScanHit.Broadcast(ScanHit);

		if (InputState.bIsTryingToInteract)
		{
			IInteractiveObject* InteractiveObject = Cast<IInteractiveObject>(ScanHit.Actor);
			InteractiveObject->InteractionEvent(this);

			InputState.bIsTryingToInteract = false;
		}	
	}
	else 
	{
		OnScanMiss.Broadcast(ScanHit);
	}
}

void AShooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InputComponent->BindAxis("MoveX", this, &AShooter::MoveInputX);
	InputComponent->BindAxis("MoveY", this, &AShooter::MoveInputY);
	InputComponent->BindAxis("LookX", this, &AShooter::LookInputX);
	InputComponent->BindAxis("LookY", this, &AShooter::LookInputY);
	InputComponent->BindAction("Vault", IE_Pressed, this, &AShooter::VaultPress);
	InputComponent->BindAction("Vault", IE_Released, this, &AShooter::VaultRelease);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AShooter::CrouchPress);
	InputComponent->BindAction("Crouch", IE_Released, this, &AShooter::CrouchRelease);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AShooter::AimPress);
	InputComponent->BindAction("Aim", IE_Released, this, &AShooter::AimRelease);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AShooter::InteractPress);
	InputComponent->BindAction("Interact", IE_Released, this, &AShooter::InteractRelease);
	InputComponent->BindAction("Swap Up", IE_Pressed, this, &AShooter::SwapPressUp);
	InputComponent->BindAction("Swap Down", IE_Pressed, this, &AShooter::SwapPressDown);
	InputComponent->BindAction("Melee", IE_Pressed, this, &AShooter::MeleePress);
	InputComponent->BindAction("Melee", IE_Released, this, &AShooter::MeleeRelease);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooter::ShootPress);
	InputComponent->BindAction("Shoot", IE_Released, this, &AShooter::ShootRelease);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AShooter::SprintPress);
	InputComponent->BindAction("Sprint", IE_Released, this, &AShooter::SprintRelease);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooter::ReloadPress);
	InputComponent->BindAction("Reload", IE_Released, this, &AShooter::ReloadRelease);
	InputComponent->BindAction("Throw Primary", IE_Pressed, this, &AShooter::ThrowPrimaryPress);
	InputComponent->BindAction("Throw Primary", IE_Released, this, &AShooter::ThrowPrimaryRelease);
	InputComponent->BindAction("Throw Secondary", IE_Pressed, this, &AShooter::ThrowSecondaryPress);
	InputComponent->BindAction("Throw Secondary", IE_Released, this, &AShooter::ThrowSecondaryRelease);
}

void AShooter::OnTriggerEnter(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(AVaultTrigger::StaticClass()))
	{
		bIsInsideVaultTrigger = true;
	}
}

void AShooter::OnTriggerExit(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(AVaultTrigger::StaticClass()))
	{
		bIsInsideVaultTrigger = false;
		// force player to stop being able to scan vault object by broadcasting a miss scan. Is there a better way we could do this?
		OnScanMiss.Broadcast(ScanHit);
	}
}

void AShooter::ShooterMakeNoise(FVector Location, float Volume)
{
	OnMakeNoise.Broadcast(Location, Volume);
}

void AShooter::MakeSound(const float Volume)
{
	FHitResult SoundHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Camera->GetForwardVector() * 10000.f;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(SoundHit, TraceStart, TraceEnd, ECC_Camera, QueryParams);

	if (bHit)
	{	
		ShooterMakeNoise(SoundHit.ImpactPoint, Volume);
	}
}

void AShooter::HandleDeath()
{
	StateMachine->SetState("Death");
}

bool AShooter::GetCanVault()
{
	return bIsInsideVaultTrigger && bIsLookingAtVaultObject;
}