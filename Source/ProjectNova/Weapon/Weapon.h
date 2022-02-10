#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Gameplay/InteractiveObject.h"
#include "Weapon.generated.h"

/** Type of locomotion to use when this weapon is held*/
UENUM()
enum ELocomotionType
{
	LT_Default,
	LT_Pistol,
	LT_Rifle
};

/** Structure holding relevant animation IK transform offset info*/
USTRUCT(BlueprintType)
struct FWeaponAnimationData
{
	GENERATED_BODY()

public:

	FWeaponAnimationData()
		:	Impulse(50.f)
		,	ImpulseFallOff(50.f)
		,	ImpulseRecovery(50.f)
		,	ImpulseKickFactor(0.3f)
		,	ImpulseVelocityMax(10.f)
		,	ImpulseMax(10.f)
	{
	}

	/** By how much do we displace the holding actors weapon mesh when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " IK")
	FVector AbsoluteLocationOffset;

	/** By how much do we rotate the holding actors weapon mesh when held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " IK")
	FRotator AbsoluteRotationOffset;

	/** What type of locomotion should be used when this weapon is held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ELocomotionType> LocomotionType;

	/** IK position offset of right hand*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " IK")
	FVector REffectorLocationOffset;

	/** IK position offset of right elbow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " IK")
	FVector RTargetLocationOffset;

	/** IK position offset of left elbow*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " IK")
	FVector LTargetLocationOffset;

	/** How much recoil is applied each time the weapon is fired*/
	UPROPERTY(EditAnywhere, Category = "Impulse")
	float Impulse;

	/** Controls how quickly the impulse decays*/
	UPROPERTY(EditAnywhere, Category = "Impulse")
	float ImpulseFallOff;

	/** Controls how quickly weapon position returns to normal after firing*/
	UPROPERTY(EditAnywhere, Category = "Impulse")
	float ImpulseRecovery;

	/** Maximum amount of impulse rate of change that can be applied to the weapon*/
	UPROPERTY(EditAnywhere, Category = "Impulse")
	float ImpulseVelocityMax;

	/** Maximum amount of impulse offset that can be applied to the weapon*/
	UPROPERTY(EditAnywhere, Category = "Impulse")
	float ImpulseMax;

	/** Factor to multiply by kick to determine rotation "kick-back" of weapon IK*/
	UPROPERTY(EditAnywhere, Category = "Impulse")
	float ImpulseKickFactor;

	/** Actual impulse value to read from weapon*/
	float ImpulseOffset;

	/** Live impulse value*/
	float ImpulseVelocity;

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

	UPROPERTY()
	int32 LoadedAmmo;

	UPROPERTY()
	int32 MaxAmmo;

	UPROPERTY()
	int32 ExcessAmmo;
};

class UCombatComponent;

UCLASS(HideCategories = ("Animation", "Sockets", "Component Tick", "Clothing", "Master Pose Component", "Physics", "Clothing Simulation", "Collision", "Rendering", "Skin Weights", "Navigation", "Virtual Texture", "Tags", "Component Replication", "Activation", "Cooking", "Skeletal Mesh", "Optimization", "Material Parameters", "HLOD", "Mobile", "Asset User Data", "Actor", "Input", "LOD"))
class PROJECTNOVA_API AWeapon : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	/** Should this weapon require a fresh attack input or should a single input be able to be held*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | General")
	uint8 bAttackContinuously : 1;

	/** Scene component used for fire traces. Typically the holding pawns camera*/
	USceneComponent* TraceOrigin;

	/** Data concerning this weapon's IK transform*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Animation")
	FWeaponAnimationData AnimData;

protected:

	/** The mesh component this weapon uses as its physical representation when not held*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon | Mesh")
	USkeletalMeshComponent* Mesh;

	/** Amount of physics force applied to the weapon when the owning pawn throws it away*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | General")
	float ThrowForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI | HUD")
	FWeaponHUD HUDData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI | Interaction")
	FInteractionPrompt Prompt;

	/** CombatComponent this weapon currently belongs to*/
	UCombatComponent* OwningComponent;

public:

	/** Sets the OwningComponent value of this weapon*/
	virtual void SetCombatComponent(UCombatComponent* NewOwner);

	/** Defines whether or not this weapon contains aiming capabilities*/
	virtual bool IsAimable() { return false; }

	/** Defines whether or not this weapon contains reload capabilities*/
	virtual bool IsReloadable() { return false; }

	/** Executes subclass implementation when receiving a press aim command*/
	virtual void StartAim() { }

	/** Executes subclass implementation when receiving a release aim command*/
	virtual void StopAim() { }

	/** Executes subclass implementation when receiving a press attack*/
	virtual void Attack() { }

	/** Executes subclass implementation when receiving a reload command*/
	virtual void Reload() { }


	virtual FInteractionPrompt& GetInteractionPrompt() override { return Prompt; }

	const FWeaponHUD& GetImmutableHUD() const { return HUDData; }

	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:

	/** Informs the combat component this weapon's properties need to be updated in the HUD*/
	virtual void NotifyHUD();

	/** Places this weapon in the event senders inventory if they contain a combat component*/
	virtual void InteractionEvent(APawn* EventSender) override;

	/** Blueprint event executed when this weapon executes its Attack function*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Attack"))
	void ReceiveAttack();

private:

	/**
	 * If true, this weapon will broadcast information
	 * Weapons that are picked up by pawns who own a HUD should mark this as true
	 * and false when dropped
	 */
	uint8 bReportHUDEvents;
};
