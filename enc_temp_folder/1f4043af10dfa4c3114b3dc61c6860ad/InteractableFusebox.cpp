// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableFusebox.h"

// Sets default values
AInteractableFusebox::AInteractableFusebox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	bCanInteract = true;
}

void AInteractableFusebox::InteractionEvent(APawn* EventSender)
{
	SetCanInteract(false);
	FuseboxEvent.Broadcast(EventSender);
	OnInteract.Broadcast(EventSender);
}

void AInteractableFusebox::SetCanInteract(bool Status)
{
	bCanInteract = Status;
}

// Called when the game starts or when spawned
void AInteractableFusebox::BeginPlay()
{
	Super::BeginPlay();
}

