#include "Gun.h"
#include "Bullet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"


FGunUIData AGun::GetGunUI() const
{
	FGunUIData Data;
	Data.AmmoInClip = CurrentAmmo;
	Data.ExcessAmmo = ExccessAmmo;
	Data.ClipSize = ClipSize;

	return Data;
}

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;

	StartingPoolSize = 3;
	ProjectileSpeed = 3000.f;

	AimFOV = 60.f;
	BarrelSocketName = "barrel";

	ClipSize = 100;
	MaxHeldAmmo = 100;
	MaxFireRange = 10000.f;

	BodyMultiplier = 1.f;
	HeadMultiplier = 2.f;
	LimbMultiplier = 0.5f;

	FireRate = 0.05f;
	Recoil = 20.f;
	RecoilAimFactor = 0.5f;
	RecoilFallOff = 300.f;
	RecoilRecovery = 30.f;
	RecoilAngularLimit = 15.f;

	BloomMax = 90.f;
	Bloom = 5.f;
	BloomFallOff = 20.f;
	BloomWalkBase = 15.f;
	BloomCrouchBase = 5.f;
	BloomProneBase = 2.5f;
	BloomBaseMovementMultiplier = 3.f;

	PrimaryActorTick.SetTickFunctionEnable(false);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	BloomMin = BloomWalkBase;

	/** Generate initial object pool*/
	for (int i = 0; i < StartingPoolSize; i++)
	{
		AActor* NewActor = GetWorld()->SpawnActor(BulletTemplate->GetDefaultObject()->GetClass());
		ABullet* NewBullet = Cast<ABullet>(NewActor);
		NewBullet->InitializeOwner(BaseDamage, BodyMultiplier, LimbMultiplier, HeadMultiplier, MaxFireRange, ProjectileSpeed);
		BulletPool.Add(NewBullet);
	}	
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCanFire)
	{
		FireTimer += DeltaTime;

		if (FireTimer >= FireRate)
		{
			bCanFire = true;
			FireTimer = 0.f;
		}
	}

	else if (bAttacking)
	{
		if (bIsAimed)
		{
			FireStraight();
		}

		else
		{
			FireWithNoise();
		}
	}

	if (RecoilVelocity > 0)
	{
		const float RecoilBleed = RecoilFallOff * DeltaTime;
		AddRecoilVelocity(-RecoilBleed);
	}

	if (CurrentBloom != BloomMin)
	{
		const float BloomBleed = BloomFallOff * DeltaTime;
		AddBloom(-BloomBleed);
	}
}

bool AGun::IsReloadable()
{
	return (ExccessAmmo > 0) && (CurrentAmmo < ClipSize);
}

void AGun::FireStraight()
{
	OnWeaponAttack.Broadcast();

	if (CurrentAmmo == 0)
	{
		StopAttack();
		return;
	}

	CurrentAmmo--;
	bCanFire = false;

	OnUpdateUI.ExecuteIfBound();

	const FVector TraceStart = TraceOrigin->GetComponentLocation();
	const FVector ProjectileStart = BulletOrigin->GetSocketLocation(ProjectileOrigin);
	const FVector TraceDirection = TraceOrigin->GetForwardVector();

	//Compute the projectile travel vector
	//Here we assume the projectile will end up exactly where a hitscan says it will
	//its possible it misses (the target moved out of the way) in which case the projectile will be far off!
	//potential solution, check if the bullet distance traveled is greater than ProjectileStart and ProjectileEndGuess
	//and correct the path if it is and hasn't collided yet
	//Lets playtest and find out
	FHitResult Hit;
	const FVector TraceEnd = TraceStart + (TraceDirection * MaxFireRange);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	const FVector ProjectileEndGuess = bHit ? Hit.ImpactPoint : TraceEnd;

	const FVector ProjectileDirection = (ProjectileEndGuess - ProjectileStart).GetSafeNormal();
	const FQuat ProjectileRotation = ProjectileDirection.ToOrientationQuat();

	//Get a bullet from the pool and send it off
	ABullet* Bullet = GetAvailableBullet();
	Bullet->SetActorLocationAndRotation(ProjectileStart, ProjectileRotation);
	Bullet->SetTrajectory(TraceStart, TraceDirection, ProjectileDirection);

	//Apply recoil
	AddRecoilVelocity(Recoil * RecoilAimFactor);

	if (WeaponFireType == EWeaponFireType::FT_Semi)
	{
		StopAttack();
	}
}

void AGun::FireWithNoise()
{
	OnWeaponAttack.Broadcast();

	if (CurrentAmmo == 0)
	{
		StopAttack();
		return;
	}

	CurrentAmmo--;
	bCanFire = false;

	OnUpdateUI.ExecuteIfBound();

	const FVector TraceStart = TraceOrigin->GetComponentLocation();
	const FVector ProjectileStart = BulletOrigin->GetSocketLocation(ProjectileOrigin);
	FVector TraceDirection = TraceOrigin->GetForwardVector();

	//Apply bloom
	const float HorizontalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;
	const float VerticalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;

	const FVector TraceXY = FMath::Lerp(TraceDirection, TraceOrigin->GetRightVector(), HorizontalRandom);
	const FVector TraceZ = FMath::Lerp(TraceDirection, TraceOrigin->GetUpVector(), VerticalRandom);
	TraceDirection = (TraceXY + TraceZ).GetSafeNormal();

	AddBloom(Bloom);
	
	//Compute the projectile travel vector
	//Here we assume the projectile will end up exactly where a hitscan says it will
	//its possible it misses (the target moved out of the way) in which case the projectile will be far off!
	//potential solution, check if the bullet distance traveled is greater than ProjectileStart and ProjectileEndGuess
	//and correct the path if it is and hasn't collided yet
	//Lets playtest and find out
	FHitResult Hit;
	const FVector TraceEnd = TraceStart + (TraceDirection * MaxFireRange);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	const FVector ProjectileEndGuess = bHit ? Hit.ImpactPoint : TraceEnd;

	const FVector ProjectileDirection = (ProjectileEndGuess - ProjectileStart).GetSafeNormal();
	const FQuat ProjectileRotation = ProjectileDirection.ToOrientationQuat();

	//Get a bullet from the pool and send it off
	ABullet* Bullet = GetAvailableBullet();
	Bullet->SetActorLocationAndRotation(ProjectileStart, ProjectileRotation);
	Bullet->SetTrajectory(TraceStart, TraceDirection, ProjectileDirection);

	AddRecoilVelocity(Recoil);

	if (WeaponFireType == EWeaponFireType::FT_Semi)
	{
		StopAttack();
	}
}

ABullet* AGun::GetAvailableBullet()
{
	//Search through the pool for the first active bullet
	for (int i = 0; i < BulletPool.Num(); i++)
	{
		if (!BulletPool[i]->GetIsActive())
		{
			return BulletPool[i];
		}
	}

	//We don't have any available bullets in the pool, create a new one then return it
	AActor* NewActor = GetWorld()->SpawnActor(BulletTemplate->GetDefaultObject()->GetClass());
	ABullet* NewBullet = Cast<ABullet>(NewActor);
	NewBullet->InitializeOwner(BaseDamage, BodyMultiplier, LimbMultiplier, HeadMultiplier, MaxFireRange, ProjectileSpeed);
	NewBullet->SetBulletQueryParams(QueryParams);
	BulletPool.Add(NewBullet);

	return NewBullet;
}

void AGun::Reload()
{
	const uint16 AmmoDifference = ClipSize - CurrentAmmo;
	const uint16 AmmoToRestore = FMath::Min(AmmoDifference, ExccessAmmo);

	CurrentAmmo += AmmoToRestore;
	ExccessAmmo -= AmmoToRestore;

	OnUpdateUI.ExecuteIfBound();
}

void AGun::AddExcessAmmo(int AmmoAddAmount)
{
	ExccessAmmo += AmmoAddAmount;
	OnUpdateUI.ExecuteIfBound();
}

void AGun::AddRecoilVelocity(const float Velocity)
{
	RecoilVelocity += Velocity;
	RecoilVelocity = FMath::Clamp(RecoilVelocity, 0.f, Recoil);
}

void AGun::AddBloom(const float BloomAmount)
{
	CurrentBloom += BloomAmount;
	CurrentBloom = FMath::Clamp(CurrentBloom, BloomMin, BloomMax);
}

void AGun::SetBloomMin(const EWeaponFireStance Stance, const bool bIsMoving)
{
	//TODO get this working agian. Fire stance was a hacky solution

	/*float Base = 0.f;

	switch (Stance)
	{
	case WFS_Standing:
		Base = BloomWalkBase;
		break;
	case WFS_Crouching:
		Base = BloomCrouchBase;
		break;
	case WFS_Proning:
		Base = BloomProneBase;
		break;
	}

	const float Multiplier = bIsMoving ? BloomBaseMovementMultiplier : 1.f;
	BloomMin = Base * Multiplier;*/
}

void AGun::SetWeaponSceneValues(USceneComponent* TraceOriginComponent, USkeletalMeshComponent* ProjectileOriginMesh)
{
	Super::SetWeaponSceneValues(TraceOriginComponent, ProjectileOriginMesh);

	//Pawn picked the weapon up
	if (TraceOriginComponent != nullptr)
	{
		//Don't shoot ourselves
		QueryParams.ClearIgnoredActors();
		//not perfect, fire location may not be directly attached to the actor we want to avoid hurting
		QueryParams.AddIgnoredActor(TraceOriginComponent->GetOwner());
		QueryParams.AddIgnoredActor(this);

		for (ABullet* Bullet : BulletPool)
		{
			Bullet->SetBulletQueryParams(QueryParams);
		}

		BulletOrigin = ProjectileOriginMesh->GetSocketByName(BarrelSocketName);
		PrimaryActorTick.SetTickFunctionEnable(true);
	}

	//Pawn dropped the weapon, stop ticking
	else
	{
		BulletOrigin = nullptr;
		PrimaryActorTick.SetTickFunctionEnable(false);
	}	
}