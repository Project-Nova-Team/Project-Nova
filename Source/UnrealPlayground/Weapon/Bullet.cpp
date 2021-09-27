#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName("NoCollision");
	SetActorEnableCollision(false);
}

void ABullet::InitializeOwner(const float Base, const float Body, const float Limb, const float Head, const float Range, const float FireSpeed)
{
	BaseDamage = Base;
	BodyMultiplier = Body;
	LimbMultiplier = Limb;
	HeadMultiplier = Head;
	MaxRange = Range;
	Speed = FireSpeed;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.SetTickFunctionEnable(false);
	bIsActive = false;
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float MaxRangeSq = MaxRange * MaxRange;
	const FVector Delta = TraceDirection * Speed * DeltaTime;
	const FVector TraceEnd = CurrentTraceLocation + Delta;

	SetActorLocation(GetActorLocation() + Delta);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, CurrentTraceLocation, TraceEnd, ECC_Pawn, QueryParams);
	
	if (Hit.bBlockingHit)
	{
		if (Hit.Actor != nullptr)
		{
			float DamageFactor = BodyMultiplier;

			if (Hit.BoneName == "Head")
			{
				DamageFactor = HeadMultiplier;
			}

			else if (Hit.BoneName == "Limb")
			{
				DamageFactor = LimbMultiplier;
			}

			const float Damage = DamageFactor * BaseDamage;
			//We don't use this but its required by Unreal's damage events
			//not sure if its safe to pass a nullptr
			const FDamageEvent DamageEvent;

			Hit.Actor->TakeDamage(Damage, DamageEvent, nullptr, this);

			//TODO implement hit splat particle effects
		}

		SetIsActive(false);
		return;
	}

	//Bullet traveled max distance
	else if (FVector::DistSquared(TraceEnd, TraceStart) >= MaxRangeSq)
	{
		SetIsActive(false);
		return;
	}

	CurrentTraceLocation = TraceEnd;
}

void ABullet::SetTrajectory(const FVector TraceOrigin, const FVector TraceVector, const FVector BulletVector)
{
	TraceStart = TraceOrigin;
	CurrentTraceLocation = TraceOrigin;
	TraceDirection = TraceVector;
	BulletDirection = BulletVector;

	SetIsActive(true);
}

void ABullet::SetIsActive(const bool Value)
{
	PrimaryActorTick.SetTickFunctionEnable(Value);
	bIsActive = Value;
	Mesh->SetVisibility(Value);
}