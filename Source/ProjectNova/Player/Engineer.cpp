#include "Engineer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "../Gameplay/HealthComponent.h"
#include "../ShooterGameMode.h"

FName AEngineer::CameraSocketName = TEXT("Camera_Socket");

AEngineer::AEngineer()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), CameraSocketName);

	Combat = CreateDefaultSubobject<UTCombatComponent>(TEXT("Combat"));
	Combat->SetUpConstruction(GetMesh(), Camera);

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	LookAroundSpeed = 45.f;
	LookUpSpeed = 450.f;

	GetCharacterMovement()->bOrientRotationToMovement = false; 
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	InteractionLookRange = 300.f;
}

void AEngineer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEngineer::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AEngineer::ScanInteractiveObject()
{
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Camera->GetForwardVector() * InteractionLookRange;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams);

	//We're looking at an object that is interactive
	if (bHit && Hit.GetActor() != nullptr && Hit.GetActor()->Implements<UInteractiveObject>())
	{
		IInteractiveObject* InteractiveObject = Cast<IInteractiveObject>(Hit.GetActor());
		
		if (InteractiveObject->CanInteract())
		{
			//We are looking at a new object, inform the HUD
			if (InteractiveObject != LastScannedObject)
			{
				OnInteractionUpdate.ExecuteIfBound(InteractiveObject);
			}

			LastScannedObject = InteractiveObject;

			if (bIsTryingToInteract)
			{
				InteractiveObject->InteractionEvent(this);
				bIsTryingToInteract = false;
			}

			
			return;
		}		
	}

	// If we were looking at something previously but aren't anymore, clear the HUD
	if (LastScannedObject == nullptr)
	{
		OnInteractionUpdate.ExecuteIfBound(nullptr);
	}

	LastScannedObject = nullptr;
}

void AEngineer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &AEngineer::MoveX);
	InputComponent->BindAxis("MoveY", this, &AEngineer::MoveY);
	InputComponent->BindAxis("LookX", this, &AEngineer::LookX);
	InputComponent->BindAxis("LookY", this, &AEngineer::LookY);
	InputComponent->BindAction("Vault", IE_Pressed, this, &AEngineer::VaultPress);
	InputComponent->BindAction("Vault", IE_Released, this, &AEngineer::VaultRelease);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AEngineer::CrouchPress);
	InputComponent->BindAction("Crouch", IE_Released, this, &AEngineer::CrouchRelease);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AEngineer::AimPress);
	InputComponent->BindAction("Aim", IE_Released, this, &AEngineer::AimRelease);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AEngineer::InteractPress);
	InputComponent->BindAction("Interact", IE_Released, this, &AEngineer::InteractRelease);
	InputComponent->BindAction("Swap Up", IE_Pressed, this, &AEngineer::SwapPressUp);
	InputComponent->BindAction("Swap Down", IE_Pressed, this, &AEngineer::SwapPressDown);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AEngineer::ShootPress);
	InputComponent->BindAction("Shoot", IE_Released, this, &AEngineer::ShootRelease);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AEngineer::SprintPress);
	InputComponent->BindAction("Sprint", IE_Released, this, &AEngineer::SprintRelease);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AEngineer::ReloadPress);
	InputComponent->BindAction("Reload", IE_Released, this, &AEngineer::ReloadRelease);
	// bind pause to game mode because pausing is not a shooter behavior
	InputComponent->BindAction("Pause", IE_Pressed, GetWorld()->GetAuthGameMode<AShooterGameMode>(), &AShooterGameMode::PauseGame)
		.bExecuteWhenPaused = true;
}

void AEngineer::MoveX(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEngineer::MoveY(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEngineer::LookX(float Value)
{
	CurrentLookX = Value;

	AddControllerYawInput(Value * LookAroundSpeed * GetWorld()->GetDeltaSeconds());
}

void AEngineer::LookY(float Value)
{
	CurrentLookY = Value;

	AngleLookY += Value * LookUpSpeed * GetWorld()->GetDeltaSeconds();
	AngleLookY = FMath::Clamp(AngleLookY, -LookVerticalClamp, LookVerticalClamp);
}

void AEngineer::SprintPress()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AEngineer::SprintRelease()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


bool AEngineer::IsRunning()
{
	const float V1 = GetCharacterMovement()->Velocity.SizeSquared();
	const float V2 = WalkSpeed * WalkSpeed;

	return GetCharacterMovement()->IsMovingOnGround() && V1 > V2;
}
