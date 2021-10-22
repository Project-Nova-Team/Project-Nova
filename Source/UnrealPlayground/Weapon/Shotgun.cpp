#include "Shotgun.h"
#include "Bullet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"

AShotgun::AShotgun()
{
	BulletsPerShot = 12;
	StartingPoolSize = BulletsPerShot;
	MaxFireRange = 700;
	WeaponFireType = EWeaponFireType::FT_Semi;
}


//NOTE these implementations are not very dry. Gun::Fire methods should probably break functionality into smaller pieces
void AShotgun::FireStraight()
{
	//Shotguns always fire with bloom
	FireWithNoise();
}

void AShotgun::FireWithNoise()
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

	for (int i = 0; i < BulletsPerShot; i++)
	{
		const FVector TraceStart = TraceOrigin->GetComponentLocation();
		const FVector ProjectileStart = BulletOrigin->GetSocketLocation(ProjectileOrigin);
		FVector TraceDirection = TraceOrigin->GetForwardVector();

		//Apply bloom
		const float HorizontalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;
		const float VerticalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;

		const FVector TraceXY = FMath::Lerp(TraceDirection, TraceOrigin->GetRightVector(), HorizontalRandom);
		const FVector TraceZ = FMath::Lerp(TraceDirection, TraceOrigin->GetUpVector(), VerticalRandom);
		TraceDirection = (TraceXY + TraceZ).GetSafeNormal();

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
	}
	
	//Apply recoil & bloom
	AddRecoilVelocity(Recoil * RecoilAimFactor);
	AddBloom(Bloom);
	AddImpulseVelocity(Impulse);

	if (WeaponFireType == EWeaponFireType::FT_Semi)
	{
		StopAttack();
	}
}