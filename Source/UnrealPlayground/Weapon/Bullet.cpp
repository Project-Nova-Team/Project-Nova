// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/Ammunition/SM_Shell_45ap.SM_Shell_45ap'"));
	UStaticMesh* Asset = MeshAsset.Object;

	SetActorEnableCollision(false);

	Mesh->SetStaticMesh(Asset);
	RootComponent = Cast<USceneComponent>(Mesh);

	FVector Scale = FVector(2, 2, 2);
	Mesh->SetWorldScale3D(Scale);

	FRotator Rotation = FRotator(0.f, -90.f, 0.f);

	Mesh->AddWorldRotation(Rotation);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RaycastHit.IsValidBlockingHit())
	{
		SetActorLocation(UKismetMathLibrary::VInterpTo_Constant(GetActorLocation(), RaycastHit.ImpactPoint, DeltaTime, 100.f));
	}
}

