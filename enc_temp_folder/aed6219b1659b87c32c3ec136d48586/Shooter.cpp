#include "Shooter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Utility/DelayedActionManager.h"
#include "../ShooterGameMode.h"
#include "../State/FPS/ShooterStateMachine.h"
#include "../Gameplay/HealthComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "ShooterInventory.h"
#include "../Animation/ShooterAnimInstance.h"
#include "../Gameplay/MeleeComponent.h"
#include "../Weapon/Gun.h"
#include "../State/State.h"
#include <ProjectNova/Gameplay/VaultObject.h>
#include "../Gameplay/HealthPickup.h"

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
	Camera = CreateDefaultSubobject<UFirstPersonCameraComponent>(TEXT("Camera"));
	ShooterMovement = CreateDefaultSubobject<UShooterMovementComponent>(TEXT("Movement"));

	SetRootComponent(Collider);
	CameraAnchor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->AttachToComponent(CameraAnchor, FAttachmentTransformRules::KeepRelativeTransform);

	Collider->SetCollisionProfileName("Pawn");
	Collider->SetCapsuleHalfHeight(ShooterMovement->StandingHeight);
	Collider->SetCapsuleRadius(ShooterMovement->CollisionRadius);
	Collider->bDynamicObstacle = true;
	CameraAnchor->SetRelativeLocation(FVector(0, 0, ShooterMovement->CameraHeight));

	ShooterSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	ShooterSkeletalMesh->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetUpConstruction(ShooterSkeletalMesh, Camera, &InputState.bIsRunning);

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	PerceptionSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus Source"));

	Inventory = CreateDefaultSubobject<UShooterInventory>(TEXT("Inventory"));
	Inventory->Shooter = this;
	Inventory->Combat = Combat;

	bInputEnabled = true;

	StartingStateOverride.Empty();
}

void AShooter::SetStateOverride(const FString NewState)
{
	StateMachine->SetState(NewState);
}

void AShooter::BeginPlay()
{
	Super::BeginPlay();

	Collider->SetCapsuleHalfHeight(ShooterMovement->StandingHeight);
	Collider->SetCapsuleRadius(ShooterMovement->CollisionRadius);
	CameraAnchor->SetRelativeLocation(FVector(0, 0, ShooterMovement->CameraHeight));

	StateMachine = NewObject<UShooterStateMachine>();
	StateMachine->Initialize(this);

	if (!StartingStateOverride.IsEmpty())
	{
		SetStateOverride(StartingStateOverride);
	}

	InputState.Owner = this;

	Health->OnDeath.AddDynamic(this, &AShooter::HandleDeath);
}

void AShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	InputState.Tick(DeltaTime);
	StateMachine->Tick(DeltaTime);
	ScanInteractiveObject();
}

void AShooter::ScanInteractiveObject()
{
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Camera->GetForwardVector() * FMath::Min(ShooterMovement->StandingHeight * 2.f, ShooterMovement->InteractionDistance);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Camera, QueryParams);

	//We're looking at an object that is interactive
	if (bHit && Hit.Actor != nullptr && Hit.Actor->Implements<UInteractiveObject>())
	{
		IInteractiveObject* InteractiveObject = Cast<IInteractiveObject>(Hit.Actor);

		//Looking at an object we were not looking at last frame
		if (InteractiveObject->CanInteract())
		{
			//If we were looking at another object last frame, tell it we stopped looking
			if (InteractiveObject != LastScannedObject)
			{
				if (LastScannedObject != nullptr)
				{
					LastScannedObject->ReceiveLookedAway(this);
				}
				//Now tell our new object we're looking at it
				InteractiveObject->ReceiveLookedAt(this);
				LastScannedObject = InteractiveObject;
				OnInteractionUpdate.ExecuteIfBound(InteractiveObject);
			}
		}


		else if (LastScannedObject == InteractiveObject)
		{
			LastScannedObject->ReceiveLookedAway(this);
			LastScannedObject = nullptr;
			OnInteractionUpdate.ExecuteIfBound(nullptr);
		}
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
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooter::ShootPress);
	InputComponent->BindAction("Shoot", IE_Released, this, &AShooter::ShootRelease);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AShooter::SprintPress);
	InputComponent->BindAction("Sprint", IE_Released, this, &AShooter::SprintRelease);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooter::ReloadPress);
	InputComponent->BindAction("Reload", IE_Released, this, &AShooter::ReloadRelease);
}

void AShooter::ShootPress()
{ 
	if (bInputEnabled)
	{
		Combat->ReceiveStartAttack();
	}
}

void AShooter::ShootRelease()
{ 
	if (bInputEnabled)
	{
		Combat->ReceiveStopAttack();
	}
}

void AShooter::AimPress()
{ 
	if (bInputEnabled)
	{
		//Combat->ReceiveAimStart(); 
	}
}

void AShooter::AimRelease()
{ 
	if (bInputEnabled)
	{
		//Combat->ReceiveAimStop(); 
	}
}

void AShooter::SwapPressUp()
{
	if (bInputEnabled)
	{
		Combat->SwapWeapon(-1);
	}
}

void AShooter::SwapPressDown()
{ 
	if (bInputEnabled)
	{
		Combat->SwapWeapon(1);
	}
}

void AShooter::ReloadPress()
{
	if (bInputEnabled)
	{
		Combat->ReceiveReload();
	}
}

void AShooter::HandleDeath()
{
	StateMachine->SetState("Death");
}

bool AShooter::IsAttacking()
{
	if (UShooterAnimInstance* ShooterAnim = Cast<UShooterAnimInstance>(ShooterSkeletalMesh->AnimScriptInstance))
	{
		return ShooterAnim->Montage_IsPlaying(ShooterAnim->MeleeAttackMontage);
	}

	return false;
}