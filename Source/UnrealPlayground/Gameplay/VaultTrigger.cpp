// Fill out your copyright notice in the Description page of Project Settings.

#include "VaultTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AVaultTrigger::AVaultTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));

	SetRootComponent(Collider);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	Arrow->AttachToComponent(Collider, FAttachmentTransformRules::KeepRelativeTransform);
}


