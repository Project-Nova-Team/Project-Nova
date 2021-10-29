#pragma once

#include "CoreMinimal.h"
#include "WeaponInput.generated.h"

enum EWeaponFireStance
{
	WFS_Standing,
	WFS_Crouching,
	WFS_Proning
};

USTRUCT(BlueprintType)
struct FWeaponInput
{
	GENERATED_BODY()

public:
	/** Whether or not the user is moving, which affects bloom*/
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsMoving : 1;

	/** Current stance of the user, which affects recoil and bloom*/
	TEnumAsByte<EWeaponFireStance> Stance;

	/** X-axis movement input*/
	UPROPERTY(BlueprintReadOnly)
	float MoveX;

	/** Y-axis movement input*/
	UPROPERTY(BlueprintReadOnly)
	float MoveY;

	/** X-axis look input*/
	UPROPERTY(BlueprintReadOnly)
	float LookX;

	/** Y-axis look input*/
	UPROPERTY(BlueprintReadOnly)
	float LookY;
};

USTRUCT(BlueprintType)
struct FShooterInventory
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PistolAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ShotgunAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RifleAmmo;	
};