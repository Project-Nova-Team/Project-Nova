#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

class AShooter;
class AWeapon;
class UCameraComponent;
class UShooterMovementComponent;
class UCombatComponent;
class UMeleeComponent;
class USVaultState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAimEvent);

UCLASS()
class UNREALPLAYGROUND_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* VaultAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SwapAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* ReloadAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AimStartAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AimStopAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* MeleeAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UCameraAnim* CameraDeathAnimation;

	//Need this to gurantee the State machine is loaded before we try and bind the event
	//Lazy implementation
	void BindVault();

protected:

	UPROPERTY(BlueprintAssignable)
	FAimEvent OnAimStart;

	UPROPERTY(BlueprintAssignable)
	FAimEvent OnAimStop;

	UFUNCTION()
	void ReceiveNewWeaponPickup(AWeapon* NewWeapon);

	UFUNCTION()
	void ReceiveNewWeaponDrop(AWeapon* NewWeapon);

private:

	USkeletalMeshComponent* ShooterMesh;

	/** Animation Hooks**/

	/** Returns whether this shooter is walking*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsWalking();

	/** Returns whether this shooter is falling*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsFalling();


	/** Execute Montages*/

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayVaultMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlaySwapMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayReloadMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAimStartMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAimStopMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StopMontageFromAttack();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAttackMontage();

	UFUNCTION()
	void MontageEnd(UAnimMontage* Montage, bool bInterupted);

	void NativeBeginPlay() override;

protected:

	/** Shooter Reference - obtained by getting owner and casting to AShooter*/
	UPROPERTY(BlueprintReadOnly)
	AShooter* Shooter;

	UPROPERTY(BlueprintReadOnly)
	UShooterMovementComponent* ShooterMovement;

	UPROPERTY(BlueprintReadOnly)
	UCombatComponent* ShooterCombat;

	UPROPERTY(BlueprintReadOnly)
	UCameraComponent* ShooterCamera;

	UPROPERTY(BlueprintReadOnly)
	UMeleeComponent* ShooterMelee;

	//Ugly
	USVaultState* Vault;
};
