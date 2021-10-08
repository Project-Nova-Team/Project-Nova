#include "Shooter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "../Utility/DelayedActionManager.h"
#include "../ShooterGameMode.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Weapon/ShooterCombatComponent.h"
#include "../Gameplay/HealthComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "../Gameplay/VaultTrigger.h"

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

	SetRootComponent(Collider);
	CameraAnchor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->AttachToComponent(CameraAnchor, FAttachmentTransformRules::KeepRelativeTransform);

	Collider->SetCollisionProfileName("Pawn");
	Collider->SetCapsuleHalfHeight(ShooterMovement->StandingHeight);
	Collider->SetCapsuleRadius(ShooterMovement->CollisionRadius);
	CameraAnchor->SetRelativeLocation(FVector(0, 0, ShooterMovement->CameraHeight));

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Arms, TEXT("WeaponSocket"));

	Combat = CreateDefaultSubobject<UShooterCombatComponent>(TEXT("Combat"));
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

	InputState.Owner = this;
	Combat->InitializeInput(&InputState);

	OnActorBeginOverlap.AddDynamic(this, &AShooter::OnTriggerEnter);
	OnActorEndOverlap.AddDynamic(this, &AShooter::OnTriggerExit);
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

	//Do we need to look at something to interact? Do we need to just be near it?

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

#if WITH_EDITOR
	if (bTraceDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Magenta, false, 0.02f);
	}
#endif
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
	InputComponent->BindAction("Swap", IE_Pressed, this, &AShooter::SwapPress);
	//InputComponent->BindAction("Swap", IE_Released, this, &AShooter::SwapRelease);
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

bool AShooter::IsWalking()
{
	return ShooterMovement->bIsOnGround && !ShooterMovement->InputVelocity.IsNearlyZero();
}

bool AShooter::IsFalling()
{
	return !ShooterMovement->bIsOnGround;
}

void AShooter::OnTriggerEnter(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(AVaultTrigger::StaticClass()))
	{
		bIsInsideVaultTrigger = true;
		UE_LOG(LogTemp, Warning, TEXT("Enter Trigger"));
	}
}

void AShooter::OnTriggerExit(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA(AVaultTrigger::StaticClass()))
	{
		bIsInsideVaultTrigger = false;
		// force player to stop being able to scan vault object by broadcasting a miss scan. Is there a better way we could do this?
		OnScanMiss.Broadcast(ScanHit);
		UE_LOG(LogTemp, Warning, TEXT("Exit Trigger"));
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
		DrawDebugSphere(GetWorld(), SoundHit.ImpactPoint, 20, 20, FColor::Blue, true);
	}
}

bool AShooter::GetCanVault()
{
	return bIsInsideVaultTrigger && bIsLookingAtVaultObject;
}

void AShooter::StartVaultAnimation()
{
	// broadcast an event here that will play an anim montage in shooter blueprint!
	// TAKING IN SCAN HIT HERE IS BAD!!!!!!! PLS FIX
	OnVaultPress.Broadcast(ScanHit);
}

void AShooter::PlayVaultMontage()
{
	float Test = Arms->GetAnimInstance()->Montage_Play(VaultAnimMontage, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("%s"), &Test);
}