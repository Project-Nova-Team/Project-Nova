#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Bullet.h"
#include "CombatComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName("Ragdoll");	
	Mesh->SetSimulatePhysics(true);

	StartingPoolSize = 3;
	ProjectileSpeed = 5000.f;
	ThrowForce = 100000.f;

	AimFOV = 60.f;

	ClipSize = 100;
	MaxHeldAmmo = 100;
	MaxFireRange = 10000.f;

	BaseDamage = 25.f;
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
}

void AWeapon::BeginPlay()
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

	PrimaryActorTick.SetTickFunctionEnable(false);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO exceptionally long delta times or small fire rates can fail
	//Since this all runs client side it may not matter
	if (!bCanFire)
	{
		FireTimer += DeltaTime;

		if (FireTimer >= FireRate)
		{
			bCanFire = true;
			FireTimer = 0.f;
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

void AWeapon::InteractionEvent(const APawn* EventSender)
{
	//If a pawn who sent the interaction has a combat component, pick the weapon up
	UCombatComponent* CombatComponent = EventSender->FindComponentByClass<UCombatComponent>();
	if (CombatComponent != nullptr)
	{
		CombatComponent->PickUpNewWeapon(this);
	}
}

FWeaponUIData AWeapon::GetWeaponUI() const
{
	FWeaponUIData Data;
	Data.AmmoInClip = CurrentAmmo;
	Data.ExcessAmmo = ExccessAmmo;
	Data.ClipSize = ClipSize;

	return Data;
}

void AWeapon::SetSceneValues(const USceneComponent* TraceOriginComponent, const USkeletalMeshComponent* HeldWeapon, const USkeletalMeshSocket* BulletSocket)
{
	//A pawn has picked the weapon up
	if (TraceOriginComponent != nullptr)
	{
		//Hide actor
		Mesh->SetVisibility(false);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Don't shoot ourselves
		QueryParams.ClearIgnoredActors();
		//not perfect, fire location may not be directly attached to the actor we want to avoid hurting
		QueryParams.AddIgnoredActor(TraceOriginComponent->GetOwner()); 
		QueryParams.AddIgnoredActor(this);

		for (ABullet* Bullet : BulletPool)
		{
			Bullet->SetBulletQueryParams(QueryParams);
		}

		PrimaryActorTick.SetTickFunctionEnable(true);
	}

	//A pawn has dropped the weapon
	else
	{
		//Move the gun a little below the camera 
		const FVector DropLocation = TraceOrigin->GetComponentLocation() - FVector(0, 0, 30.f);
		SetActorLocation(DropLocation);

		//Display actor
		Mesh->SetVisibility(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);
		
		//Apply a force to make it look like the gun was thrown
		Mesh->AddForce(TraceOrigin->GetForwardVector().GetSafeNormal2D() * ThrowForce);

		PrimaryActorTick.SetTickFunctionEnable(false);
	}

	TraceOrigin = TraceOriginComponent;
	HeldWeaponMesh = HeldWeapon;
	BulletOrigin = BulletSocket;
}

void AWeapon::FireStraight()
{
	if (!bCanFire)
	{
		return;
	}

	OnWeaponFire.Broadcast();

	if (CurrentAmmo == 0)
	{
		return;
	}

	CurrentAmmo--;
	bCanFire = false;
	
	const FVector TraceStart = TraceOrigin->GetComponentLocation();
	const FVector TraceDirection = TraceOrigin->GetForwardVector();
	const FVector TraceEnd = TraceStart + TraceDirection * MaxFireRange;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	
	if (Hit.IsValidBlockingHit() && Hit.Actor != nullptr)
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
		const FDamageEvent DamageEvent;
		
		Hit.Actor->TakeDamage(Damage, DamageEvent, nullptr, this);
	}

#if WITH_EDITOR
	if (bTraceDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, Hit.ImpactPoint, FColor::Red, true);
	}
#endif
}

void AWeapon::FireWithNoise(const bool bIsAimed, FRotator BulletRotation)
{
	if (!bCanFire)
	{
		return;
	}

	OnWeaponFire.Broadcast();

	if (CurrentAmmo == 0)
	{
		return;
	}

	CurrentAmmo--;
	bCanFire = false;

	const FVector TraceStart = TraceOrigin->GetComponentLocation();
	const FVector ProjectileStart = BulletOrigin->GetSocketLocation(HeldWeaponMesh);
	FVector TraceDirection = TraceOrigin->GetForwardVector();

	//If weapon is not aimed, apply bloom
	if (!bIsAimed)
	{
		const float HorizontalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;
		const float VerticalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;

		const FVector TraceXY = FMath::Lerp(TraceDirection, TraceOrigin->GetRightVector(), HorizontalRandom);
		const FVector TraceZ = FMath::Lerp(TraceDirection, TraceOrigin->GetUpVector(), VerticalRandom);
		TraceDirection = (TraceXY + TraceZ).GetSafeNormal();

		AddBloom(Bloom);
	}

	//Compute the projectile travel vector
	//Here we assume the projectile will end up exactly where a hitscan says it will
	//its possible it misses (the target moved out of the way) in which case the projectile will be far off!
	//potential solution, check if the bullet distance traveled is greater than ProjectileStart and ProjectileEndGuess
	//and correct the path if it is and hasn't collided yet
	//Lets playtest and find out
	FHitResult Hit;
	const FVector TraceEnd = TraceStart + (TraceDirection * MaxFireRange);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, QueryParams);
	const FVector ProjectileEndGuess = bHit ? Hit.ImpactPoint : TraceEnd;

	const FVector ProjectileDirection = (ProjectileEndGuess - ProjectileStart).GetSafeNormal();
	const FQuat ProjectileRotation = ProjectileDirection.ToOrientationQuat();

	//Get a bullet from the pool and send it off
	ABullet* Bullet = GetAvailableBullet();
	Bullet->SetActorLocationAndRotation(ProjectileStart, ProjectileRotation);
	Bullet->SetTrajectory(TraceStart, TraceDirection, ProjectileDirection);
	
	//Apply recoil
	const float RecoilFactor = bIsAimed ? RecoilAimFactor : 1.f;
	AddRecoilVelocity(Recoil * RecoilFactor);
}

ABullet* AWeapon::GetAvailableBullet()
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

void AWeapon::Reload()
{
	const uint16 AmmoDifference = ClipSize - CurrentAmmo;
	const uint16 AmmoToRestore = FMath::Min(AmmoDifference, ExccessAmmo);

	CurrentAmmo += AmmoToRestore;
	ExccessAmmo -= AmmoToRestore;
}

void AWeapon::AddExcessAmmo(int AmmoAddAmount)
{
	ExccessAmmo += AmmoAddAmount;
}

void AWeapon::AddRecoilVelocity(const float Velocity)
{
	RecoilVelocity += Velocity;
	RecoilVelocity = FMath::Clamp(RecoilVelocity, 0.f, Recoil);
}

void AWeapon::AddBloom(const float BloomAmount)
{
	CurrentBloom += BloomAmount;
	CurrentBloom = FMath::Clamp(CurrentBloom, BloomMin, BloomMax);
}

void AWeapon::SetBloomMin(const EWeaponFireStance Stance, const bool bIsMoving)
{
	float Base = 0.f;

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
	BloomMin = Base * Multiplier;
}