#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Gameplay/InteractiveObject.h"
#include "Materials/MaterialInstance.h"
#include "Weapon.generated.h"

class USkeletalMesh;
class USkeletalMeshSocket;
class UCombatComponent;
class USoundBase;
struct FWeaponInput;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponEvent);
DECLARE_DELEGATE(FWeaponUIEvent);

USTRUCT(BlueprintType)
struct FWeaponUIData
{
	GENERATED_BODY()
	
public:
};

UCLASS()
class PROJECTNOVA_API AWeapon : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	void InteractionEvent(APawn* EventSender) override;

	/** TODO stop inlining once we get some fields in FWeaponUIData*/
	FWeaponUIData GetWeaponUI() const { return FWeaponUIData(); }

	/** Returns the mesh this weapon uses*/
	FORCEINLINE USkeletalMeshComponent* GetMeshComponent() const { return Mesh; }

	/** Returns the mesh's skeletal mesh this weapon uses*/
	FORCEINLINE USkeletalMesh* GetSkeletalMesh() const { return Mesh->SkeletalMesh; }

	/** Returns the combat component this weapon is attached to, if any*/
	FORCEINLINE UCombatComponent* GetOwningComponent() const { return OwningComponent; }

	virtual bool IsReloadable() { return false; }

	UFUNCTION(BlueprintCallable)
	virtual bool IsAimable() { return false; }

	/** Initiates an attack. Returns true if the attack was executed successfully.*/
	virtual void StartAttack() { }

	/** Called by CombatComponent when attack input has been removed*/
	virtual void StopAttack() { }

	/** Sets the aim status of the weapon*/
	virtual void SetAim(const bool bNewAim) { bIsAimed = bNewAim; }

	virtual void Reload() { }

	/**
	 * Passes references of scene components to the weapon so it knows where to begin weapon fire/traces from
	 *
	 * @param	TraceOriginComponent				Scene component a traveling hitscan beam is fired from. Typically the camera of a player
	 * @param	ProjectileOrigin					Scene component we use to get a socket on a gun barrel to fire a cosmetic projectile from
	 */
	virtual void SetWeaponSceneValues(USceneComponent* TraceOriginComponent, USkeletalMeshComponent* ProjectileOrigin);

	/**
	* Pointer to a level object that determines where the line trace begins for firing
	* For the shooter, this is the camera component
	*/
	USceneComponent* TraceOrigin;

	/**
	 * Pointer to a the skeletal mesh component of the weapon's owner. This is provided
	 * so we can access a socket on the mesh to fire a cosmetic projectile from 
	 */
	USkeletalMeshComponent* ProjectileOriginMesh;

	/** Pointer to a stance enum that weapons use to vary things like bloom*/
	FWeaponInput* OwnerInput;

	/** When invoked, any HUD subscribed to this event will update values*/
	FWeaponUIEvent OnUpdateUI;

	/** Invoked when the weapon attacks. Useful for audio*/
	UPROPERTY(BlueprintAssignable)
	FWeaponEvent OnWeaponAttack;

	/** The materials that we set when the player picks up the weapon, so that it doesn't clip through walls*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | OverlapMaterials")
	TArray<UMaterialInstance*> OverlapRenderMaterials;

	/** By how much do we displace the holding actors weapon mesh when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FVector WeaponMeshOffset;

	/** By how much do we rotate the holding actors weapon mesh when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FRotator WeaponMeshRotationOffset;

	/** How much do we displace the holding actors weapon socket when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FVector REffectorIK;

	/** Right hand target IK position*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FVector RTargetIK;

	/** Left hand target IK position, Effector location is determiend by the actual socket*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FVector LTargetIK;

	/** How much is the right hand weapon socket rotated while this is held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FRotator RHandRotationIK;

	/** How much to factor rotational "kick" from impulse. Lower values provide less kick*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	float ImpulseKickFactor;

	FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

protected:

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	/** Force applied to weapon when dropped by a pawn*/
	UPROPERTY(EditAnywhere, Category = "Weapon | General")
	float ThrowForce;

	/** The amount of damage dealt by each attack withs this weapon*/
	UPROPERTY(EditAnywhere, Category = "Weapon | Damage")
	float BaseDamage;

	/** Sound this object plays when reloaded*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Audio")
	USoundBase* SFXReload;

	/** Sound this object makes when switched to*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Audio")
	USoundBase* SFXSwap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FInteractionPrompt Prompt;

	UCombatComponent* OwningComponent;

	uint8 bIsAimed : 1;
};
