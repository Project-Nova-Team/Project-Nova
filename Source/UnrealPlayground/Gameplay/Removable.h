// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Removable.generated.h"

class AActor;
class UStaticMeshComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URemovable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that are 'removable' (destroyable). Things like ammo/health pickups, triggers etc.
 */
class UNREALPLAYGROUND_API IRemovable
{
	GENERATED_BODY()

public:
	virtual void RemoveSelf(AActor* Actor);

	virtual void RestoreSelf(AActor* Actor);
};
