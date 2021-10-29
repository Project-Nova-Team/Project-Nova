#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShooterMovementComponent.h"
#include "WeaponInput.h"
#include "../Weapon/CombatComponent.h"
#include "Shooter.generated.h"

class UCapsuleComponent;
class UCameraComponent;
class UShooterStateMachine;
class UMeleeComponent;
class UHealthComponent;
class UAIPerceptionStimuliSourceComponent;
class IInteractiveObject;
class AGun;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScanEvent, FHitResult, ScanData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShooterMakeNoise, FVector, Location, float, Volume);


struct FShooterInput : public FWeaponInput
{
	FShooterInput() { }

	/** 
	* Runs within the shooter's Tick to handle any input states that need to be watched 
	* @param	DeltaTime				Time slice
	*/
	void Tick(const float DeltaTime);

	/** Watches the crouch input values so any substates can avoid re-implementing the same checks for crouching/prone*/
	void HandleCrouchInputStates(const float DeltaTime);

	/** Shooter this input state applies to*/
	AShooter* Owner;

	/** How long crouch has been held for since the last release*/
	float CurrentCrouchHoldTime;

	/** Whether or not the player is pressing the jump button*/
	uint8 bIsTryingToVault : 1;

	/** Whether or not the player is pressing the interact button*/
	uint8 bIsTryingToInteract : 1;

	/** Whether or not the player is pressing the crouch button*/
	uint8 bIsHoldingCrouch : 1;

	/** The previous ticks's state of the crouch input*/
	uint8 bWasHoldingCrouch: 1;

	/** Whether or not crouch input has been released after its been pressed*/
	uint8 bIsTryingToCrouch : 1;

	/** Whether or not crouch input has been held long enough go prone*/
	uint8 bIsTryingToProne : 1;

	/** Whether or not the player is pressing the sprint button*/
	uint8 bIsTryingToSprint : 1;
};

UCLASS()
class UNREALPLAYGROUND_API AShooter : public APawn
{
	GENERATED_BODY()

public:
	AShooter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	friend struct FShooterInput;

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	/** Returns the capsule component attached to this shooter*/
	FORCEINLINE UCapsuleComponent* GetCollider() const { return Collider; }

	/** Returns the camera anchor component attached to this shooter*/
	FORCEINLINE USceneComponent* GetAnchor() const { return CameraAnchor; }
	
	/** Returns the combat component attached to this shooter*/
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	/** Returns the camera component attached to this shooter*/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	/** Returns the state machine attached to the shooter which drives player movement*/
	FORCEINLINE UShooterStateMachine* GetStateMachine() const { return StateMachine; }

	FORCEINLINE UHealthComponent* GetHealth() const { return Health; }

	/** Returns the input state. Note: contents are mutable*/
	FORCEINLINE FShooterInput* GetInput() { return &InputState; }

	/** Returns the input state. Note: contents are mutable*/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FWeaponInput GetInputRaw() { return FWeaponInput(InputState); }

	/** Returns the Skeletal Mesh component of this shooter*/
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMeshComponent() { return ShooterSkeletalMesh; }

	/** Returns the Shooter Movement component attached to this shooter*/
	FORCEINLINE UShooterMovementComponent* GetShooterMovement() { return ShooterMovement; }

	/** Returns the Melee component attached to this shooter*/
	FORCEINLINE UMeleeComponent* GetMelee() { return Melee; }

	/** Returns the Perception Source component which enables AI to sense stimulus produced by this actor*/
	FORCEINLINE UAIPerceptionStimuliSourceComponent* GetPerceptionSource() const { return PerceptionSource; }

	/** Returns State Machine. Maybe consider moving this to shooter movement component?*/
	FORCEINLINE UShooterStateMachine* GetStateMachine() { return StateMachine; }

	/** Draws debug traces for a variety of position tests if enabled*/
	UPROPERTY(Category = Pawn, EditAnywhere)
	uint8 bTraceDebug : 1;

	/** Invoked when the shooter is looking at an object that can be interacted with (a weapon/button)*/
	UPROPERTY(BlueprintAssignable)
	FScanEvent OnScanHit;

	/** Invoked when the shooter is not looking at an object that can be interacted with (a weapon/button)*/
	UPROPERTY(BlueprintAssignable)
	FScanEvent OnScanMiss;

	/** Invoked when the shooter shooter makes a sound*/
	UPROPERTY(BlueprintAssignable)
	FShooterMakeNoise OnMakeNoise;

	/** Set when player overlaps or unoverlaps vault trigger*/
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsInsideVaultTrigger : 1;

	/** Is player looking at a vault object?*/
	UPROPERTY(BlueprintReadWrite)
	uint8 bIsLookingAtVaultObject : 1;

	/** Returns whether player is in vault trigger and is looking at vault object*/
	UFUNCTION(BlueprintCallable)
	bool GetCanVault();

	UFUNCTION(BlueprintCallable)
	void ShooterMakeNoise(FVector Location, float Volume);

	/** Sets the death state in the state machine when the shooter dies*/
	UFUNCTION()
	void HandleDeath();

protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnTriggerEnter(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnTriggerExit(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShooterSkeletalMesh;

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UShooterMovementComponent* ShooterMovement;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* Collider;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraAnchor;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeleeComponent* Melee;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionStimuliSourceComponent* PerceptionSource;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* Health;

	UPROPERTY(Transient)
	UShooterStateMachine* StateMachine;
	
	/**Contains all relevant information of the input state*/
	FShooterInput InputState;

	/**Data regarding whether or not we are looking at a weapon or interactive object*/
	FHitResult ScanHit;

	/** Casts a trace from the camera to see if there is an object nearby we can interact with*/	
	void ScanInteractiveObject();

	

	///		 Begin Input Bindings	   ///
	void MoveInputX(const float Value)	{ InputState.MoveX = Value; }
	void MoveInputY(const float Value)	{ InputState.MoveY = Value; }
	void LookInputX(const float Value)	{ InputState.LookX = Value; }
	void LookInputY(const float Value)	{ InputState.LookY = Value; }
	void VaultPress()					{ InputState.bIsTryingToVault = true; }
	void VaultRelease()					{ InputState.bIsTryingToVault = false; }
	void CrouchPress()					{ InputState.bIsHoldingCrouch = true; }
	void CrouchRelease()				{ InputState.bIsHoldingCrouch = false; }
	void ShootPress()					{ Combat->ReceiveAttack(true); }
	void ShootRelease()					{ Combat->ReceiveAttack(false); }
	void AimPress()						{ Combat->ReceiveAim(); }
	void AimRelease()					{  }
	void InteractPress()				{ InputState.bIsTryingToInteract = true; }
	void InteractRelease()				{ InputState.bIsTryingToInteract = false; }
	void SwapPressUp()					{ Combat->ReceiveSwap(-1); }
	void SwapPressDown()				{ Combat->ReceiveSwap(1); }
	void SprintPress()					{ InputState.bIsTryingToSprint = true; }
	void SprintRelease()				{ InputState.bIsTryingToSprint = false; }
	void ReloadPress()					{ Combat->ReceiveReload(); }
	void ReloadRelease()				{  }

	//TODO delete all of this
#if WITH_EDITOR
	UPROPERTY(EditAnywhere, Category = "SOUND TEST")
	float NoiseAmount;

	float NoiseIntensity;

	void MakeSound(const float Volume);
	
#endif
};