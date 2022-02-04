#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Weapon/TCombatComponent.h"
#include "../Gameplay/InteractiveObject.h"
#include "Engineer.generated.h"

class UCameraComponent;
class UHealthComponent;

DECLARE_DELEGATE_OneParam(FScan, IInteractiveObject*);


UCLASS()
class PROJECTNOVA_API AEngineer : public ACharacter
{
	GENERATED_BODY()

public:
	AEngineer();

	static FName CameraSocketName;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Invoked when we want to inform the UI we're stopped/started looking at an interactive object*/
	FScan OnInteractionUpdate;

protected:
	
	/** Camera attached to this character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	/** Combat component used for holding weapons*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UTCombatComponent* Combat;

	/** Health component for taking damage*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	UHealthComponent* Health;

	/// Control ///

	/** How fast does the character look around*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float LookAroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	/** How fast does the character look up*/
	float LookUpSpeed;

	/** Max angle that can be looked up or down*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float LookVerticalClamp;

	/** Max default movement units*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float WalkSpeed;

	/** Max movement units per second when holding run*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float RunSpeed;

	/** How many units away can we interact with interactive objects from*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float InteractionLookRange;

	
	/// Animation fields ///

	/** Current input data for LookY, used in weapon sway*/
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float CurrentLookY;

	/** Current input data for LookY, used in weapon sway*/
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float CurrentLookX;

	/** Current input data for LookY, used in weapon sway*/
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float AngleLookY;

private:

	uint8 bIsTryingToVault : 1;

	uint8 bIsTryingToInteract : 1;
	
	/** Can't get a weak object ptr to compile?? */
	IInteractiveObject* LastScannedObject;

public:	
	
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return Camera; }

	FORCEINLINE UTCombatComponent* GetCombatComponent() const { return Combat; }

	FORCEINLINE UHealthComponent* GetHealthComponent() const { return Health; }

	UFUNCTION(BlueprintCallable)
	bool IsRunning();

protected:

	void BeginPlay() override;

	/** Cast a query ray from the camera looking for InteractiveObjects. Looking at one exposes interaction functionality*/
	void ScanInteractiveObject();

private:

	///		 Begin Input Bindings	   ///

	void MoveX(const float Value);
	void MoveY(const float Value);
	void LookX(const float Value);
	void LookY(const float Value);
	void SprintPress();
	void SprintRelease();
	void VaultPress()					{ bIsTryingToVault = true; }
	void VaultRelease()					{ bIsTryingToVault = false; }
	void CrouchPress()					{ Crouch(); }
	void CrouchRelease()				{ UnCrouch(); }
	void ShootPress()					{ Combat->ReceiveStartAttack(); }
	void ShootRelease()					{ Combat->ReceiveStopAttack(); }
	void AimPress()						{ Combat->ReceiveAimStart(); }
	void AimRelease()					{ Combat->ReceiveAimStart(); }
	void InteractPress()				{ bIsTryingToInteract = true; }
	void InteractRelease()				{ bIsTryingToInteract = false; }
	void SwapPressUp()					{ Combat->SwapWeapon(-1); }
	void SwapPressDown()				{ Combat->SwapWeapon(1); }
	void ReloadPress()					{ Combat->ReceiveReload(); }
	void ReloadRelease()				{  }
};
