// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultObject.h"

// Sets default values
AVaultObject::AVaultObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AVaultObject::InteractionEvent(const APawn* EventSender)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted with vault object"));
}
