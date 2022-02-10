#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShooterMovementComponent.h"
#include "WeaponInput.h"
#include "FirstPersonCameraComponent.h"
#include "../Weapon/CombatComponent.h"
#include "../Gameplay/InteractiveObject.h"
#include "Shooter.generated.h"

class UCapsuleComponent;
class UShooterStateMachine;
class UHealthComponent;
class UAIPerceptionStimuliSourceComponent;

DECLARE_DELEGATE_OneParam(FScan, IInteractiveObject*);

struct FShooterInput
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

	/** Is velocity applied to the shooter*/
	uint8 bIsMoving : 1;

	/** X-axis movement input*/
	float MoveX;

	/** Y-axis movement input*/
	float MoveY;

	/** X-axis look input*/
	float LookX;

	/** Y-axis look input*/
	float LookY;

	/** @todo move this somewhere better*/
	bool bIsRunning;
};

UCLASS()
class PROJECTNOVA_API AShooter : public APawn
{
	GENERATED_BODY()

public:
	AShooter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	friend struct FShooterInput;

	/** Returns the capsule component attached to this shooter*/
	FORCEINLINE UCapsuleComponent* GetCollider() const { return Collider; }

	/** Returns the camera anchor component attached to this shooter*/
	FORCEINLINE USceneComponent* GetAnchor() const { return CameraAnchor; }
	
	/** Returns the combat component attached to this shooter*/
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	/** Returns the camera component attached to this shooter*/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	/** Returns the camera component as the first person camera - need this because of bad open-close violation*/
	FORCEINLINE UFirstPersonCameraComponent* GetCameraCast() const { return Camera; }

	/** Returns the state machine attached to the shooter which drives player movement*/
	FORCEINLINE UShooterStateMachine* GetStateMachine() const { return StateMachine; }

	FORCEINLINE UHealthComponent* GetHealth() const { return Health; }

	/** Returns the input state. Note: contents are mutable*/
	FORCEINLINE FShooterInput* GetInput() { return &InputState; }

	/** Returns the Skeletal Mesh component of this shooter*/
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMeshComponent() const { return ShooterSkeletalMesh; }

	/** Returns the Shooter Movement component attached to this shooter*/
	FORCEINLINE UShooterMovementComponent* GetShooterMovement() const { return ShooterMovement; }

	/** Returns the Perception Source component which enables AI to sense stimulus produced by this actor*/
	FORCEINLINE UAIPerceptionStimuliSourceComponent* GetPerceptionSource() const { return PerceptionSource; }

	/** Returns State Machine. Maybe consider moving this to shooter movement component?*/
	FORCEINLINE UShooterStateMachine* GetStateMachine() { return StateMachine; }

	/** Draws debug traces for a variety of position tests if enabled*/
	UPROPERTY(Category = Pawn, EditAnywhere)
	uint8 bTraceDebug : 1;

	/** Set when player overlaps or unoverlaps vault trigger*/
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsInsideVaultTrigger : 1;

	/** Is player looking at a vault object?*/
	UPROPERTY(BlueprintReadWrite)
	uint8 bIsLookingAtVaultObject : 1;

	/** Returns whether player is in vault trigger and is looking at vault object*/
	UFUNCTION(BlueprintCallable)
	bool CanVault();

	/** Sets the death state in the state machine when the shooter dies*/
	UFUNCTION()
	void HandleDeath();

	/** Is the shooter currently in a UI prompt event*/
	UPROPERTY(BlueprintReadWrite)
	uint8 bIsPrompted : 1;

	/** Forces state to change to the given state name*/
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetStateOverride(const FString NewState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	FString StartingStateOverride;

	/** Delegate executed when scan detects a change and we should update UI prompt*/
	FScan OnInteractionUpdate;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ShooterSkeletalMesh;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UShooterMovementComponent* ShooterMovement;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* Collider;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraAnchor;

	UPROPERTY(Category = Shooter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UFirstPersonCameraComponent* Camera;

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

	/** Look result from previous Tick*/
	IInteractiveObject* LastScannedObject;

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
	void ShootPress()					{ Combat->ReceiveStartAttack(); }
	void ShootRelease()					{ Combat->ReceiveStopAttack(); }
	void AimPress()						{ Combat->ReceiveAimStart(); }
	void AimRelease()					{ Combat->ReceiveAimStop(); }
	void InteractPress()				{ InputState.bIsTryingToInteract = true; }
	void InteractRelease()				{ InputState.bIsTryingToInteract = false; }
	void SwapPressUp()					{ Combat->SwapWeapon(-1); }
	void SwapPressDown()				{ Combat->SwapWeapon(1); }
	void SprintPress()					{ InputState.bIsTryingToSprint = true; }
	void SprintRelease()				{ InputState.bIsTryingToSprint = false; }
	void ReloadPress()					{ Combat->ReceiveReload(); }
	void ReloadRelease()				{  }
};