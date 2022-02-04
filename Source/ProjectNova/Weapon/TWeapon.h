#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Gameplay/InteractiveObject.h"
#include "TWeapon.generated.h"

/** Structure holding relevant animation IK transform offset info*/
USTRUCT(BlueprintType)
struct FWeaponIKData
{
	GENERATED_BODY()

public:

	FWeaponIKData() 
		: bUseData(true)
		, ImpulseKickFactor(0.3f)
	{

	}

	/** Whether or not any of this structs information is relevant.
	 *	Setting this to false essentially zeros out every value
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bUseData : 1;

	/** By how much do we displace the holding actors weapon mesh when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AbsoluteLocationOffset;

	/** By how much do we rotate the holding actors weapon mesh when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator AbsoluteRotationOffset;

protected:

	/** IK position offset of right hand*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector REffectorLocationOffset;

	/** IK position offset of right elbow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RTargetLocationOffset;

	/** IK rotation offset of right hand*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator REffectorRotationOffset;

	/** IK rotation offset of right elbow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RTargetRotationOffset;

	/** IK position offset of left hand*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LEffectorLocationOffset;

	/** IK position offset of left elbow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LTargetLocationOffset;

	/** IK rotation offset of left hand*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator LEffectorRotationOffset;

	/** IK rotation offset of left elbow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator LTargetRotationOffset;
	
	/** How much to factor rotational "kick" from impulse. Lower values provide less kick
	 *	This should only be set on projectile weapons like guns
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseKickFactor;
};

/** Struct package that is sent to a HUD class */
USTRUCT(BlueprintType)
struct FWeaponHUD
{
	GENERATED_BODY()

public:

	FWeaponHUD() { }

	/** Image sprite when this weapon is held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* HUDTexture;

	/** Reticle sprite when this weapon is held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ReticleTexture;
};



class UTCombatComponent;

UCLASS()
class PROJECTNOVA_API ATWeapon : public AActor, public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATWeapon();

	/** Should this weapon require a fresh attack input or should a single input be able to be held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | General")
	uint8 bAttackContinuously : 1;

protected:

    /** The mesh component this weapon uses as its physical representation when not held*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Mesh")
	USkeletalMeshComponent* Mesh;

	/** Amount of physics force applied to the weapon when the owning pawn throws it away*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | General")
	float ThrowForce;

	/** Data concerning this weapon's IK transform*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Animation")
	FWeaponIKData IKData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | UI")
	FWeaponHUD HUDData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Interaction")
	FInteractionPrompt Prompt;

	/** CombatComponent this weapon currently belongs to*/
	UTCombatComponent* OwningComponent;

public:	

	/** Sets the OwningComponent value of this weapon*/
	virtual void SetCombatComponent(UTCombatComponent* NewOwner);

	/** Defines whether or not this weapon contains aiming capabilities*/
	virtual bool IsAimable() { return false; }

	/** Defines whether or not this weapon contains reload capabilities*/
	virtual bool IsReloadable() { return false; }

	/** Executes subclass implementation when receiving a press aim command*/
	virtual void StartAim()		{ }

	/** Executes subclass implementation when receiving a release aim command*/
	virtual void StopAim()		{ }

	/** Executes subclass implementation when receiving a press attack*/
	virtual void Attack()	{ }

	/** Executes subclass implementation when receiving a reload command*/
	virtual void Reload()		{ }

	
	virtual const FInteractionPrompt& GetInteractionPrompt() const override { return Prompt; }

	const FWeaponHUD& GetImmutableHUD() const { return HUDData; }

	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:

	/** Informs the combat component this weapon's properties need to be updated in the HUD*/
	virtual void NotifyHUD() const;

	virtual void InteractionEvent(APawn* EventSender) override;

private:

	/**
	 * If true, this weapon will broadcast information
	 * Weapons that are picked up by pawns who own a HUD should mark this as true
	 * and false when dropped
	 */
	uint8 bReportHUDEvents;
};
