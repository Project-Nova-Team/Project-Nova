#pragma once

#include "CoreMinimal.h"

enum EWeaponFireStance
{
	WFS_Standing,
	WFS_Crouching,
	WFS_Proning
};

struct FWeaponInput
{
	/** Whether or not the user is trying to aim*/
	uint8 bIsTryingToAim : 1;
	
	/** Whether or not the user is trying to fire*/
	uint8 bIsTryingToFire : 1;

	/** Whether or not the user is trying to reload*/
	uint8 bIsTryingToReload : 1;

	/** Whether or not the user is trying to swap*/
	uint8 bIsTryingToSwap : 1;

	/** Whether or not the user is moving, which affects bloom*/
	uint8 bIsMoving : 1;

	/** Current stance of the user, which affects recoil and bloom*/
	TEnumAsByte<EWeaponFireStance> Stance;
};