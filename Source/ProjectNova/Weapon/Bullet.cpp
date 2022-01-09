#include "Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName("NoCollision");
	SetActorEnableCollision(false);
}

void ABullet::InitializeOwner(AGun* WeaponOwner, float Damage, float Range, float ProjectileSpeed)
{
	Gun = WeaponOwner;

	BaseDamage = Damage;
	MaxRange = Range;
	Speed = ProjectileSpeed;
	BodyMultiplier = 1;
	LimbMultiplier = 1;
	HeadMultiplier = 1;	
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
	const FVector DeltaTrace = TraceDirection * Speed * DeltaTime;
	const FVector DeltaProjectile = BulletDirection * Speed * DeltaTime;
	const FVector TraceEnd = CurrentTraceLocation + DeltaTrace;

	SetActorLocation(GetActorLocation() + DeltaProjectile);

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
			OnBulletImpact.Broadcast(Hit.Actor.Get());
			//TODO implement hit splat particle effects
			
		}
		
		//Play sound effects if we hit a brush
		else
		{
			OnBulletImpact.Broadcast(nullptr);
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