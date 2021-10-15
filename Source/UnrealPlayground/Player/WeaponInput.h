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
	/** Whether or not the user is moving, which affects bloom*/
	uint8 bIsMoving : 1;

	/**
	* Whether or not the player is pressing the interact button
	* Used to pick up items, press buttons, etc
	*/
	uint8 bIsTryingToInteract : 1;

	/** Whether or not the player is pressing the melee button*/
	uint8 bIsTryingToMelee : 1;

	/**
	*  Whether or not the player is pressing the throw primary button
	*  Used for grenades, throwing knives, etc.
	*/
	uint8 bIsTryingToThrowPrimary : 1;

	/**
	* Whether or not the player is pressing the throw secondary button
	* Used for flashbangs, concussions, etc.
	*/
	uint8 bIsTryingToThrowSecondary : 1;

	/** Current stance of the user, which affects recoil and bloom*/
	TEnumAsByte<EWeaponFireStance> Stance;
};