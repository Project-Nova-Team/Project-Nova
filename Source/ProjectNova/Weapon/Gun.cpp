#include "Gun.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

FName AGun::BarrelSocketName = TEXT("Barrel_Socket");

AGun::AGun()
	: BulletsPerFire(1)
	, bCanFire(true)
	, AngularSpread(3.f)
	, Recoil(100)
	, RecoilRecovery(100)
	, RecoilFallOff(100)
	, RecoilLimit(100)
	, RecoilAimFactor(0.5f)
{
	PrimaryActorTick.bCanEverTick = true;
	RecoilAngularLimit = 10.f;
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//First count down our fire cooldown if we need to
	if (!bCanFire)
	{
		const float TimeNow = GetWorld()->GetTimeSeconds();

		if (TimeNow - LastFireTimeStamp > FireRate)
		{
			bCanFire = true;
		}
	}

	//If theres any active recoil we need to decrease the impulse
	if (ActiveRecoil > 0.f)
	{
		ActiveRecoil -= DeltaTime * RecoilFallOff;
		ActiveRecoil = FMath::Min(ActiveRecoil, RecoilLimit);
	}

	if (AnimData.ImpulseVelocity > 0)
	{
		AnimData.ImpulseVelocity -= AnimData.ImpulseFallOff * DeltaTime;
		AnimData.ImpulseVelocity = FMath::Max(AnimData.ImpulseVelocity, 0.f);

		AnimData.ImpulseOffset += AnimData.ImpulseVelocity * DeltaTime;
		AnimData.ImpulseOffset = FMath::Min(AnimData.ImpulseOffset, AnimData.ImpulseMax);
	}

	else if (AnimData.ImpulseOffset != 0)
	{
		AnimData.ImpulseOffset -= AnimData.ImpulseRecovery * DeltaTime;
		AnimData.ImpulseOffset = FMath::Max(AnimData.ImpulseOffset, 0.f);
	}
}

void AGun::Reload()
{
	const int32 AmmoDifference = MaxAmmo - LoadedAmmo;
	const int32 AmmoToRestore = FMath::Min(AmmoDifference, ExcessAmmo);

	LoadedAmmo += AmmoToRestore;
	ExcessAmmo -= AmmoToRestore;

	NotifyHUD();
}

void AGun::AddAmmo(int32 Amount)
{
	ExcessAmmo = FMath::Min(ExcessAmmo + Amount, MaxExcessAmmo);
}

void AGun::Attack()
{
	if (!bCanFire)
	{
		return;
	}

	ReceiveAttack();

	if (LoadedAmmo < 1)
	{
		return;
	}

	--LoadedAmmo;
	bCanFire = false;
	LastFireTimeStamp = GetWorld()->GetTimeSeconds();

	Fire();
	AddRecoilVelocity();
	AddImpulseVelocity();
	NotifyHUD();
}

void AGun::Fire()
{
	//Build query params once
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TraceOrigin->GetOwner());

	//If we're only shooting one bullet we can perform a faster test
	if (BulletsPerFire == 1)
	{
		FHitResult Hit;

		const float HorizontalRandom = FMath::FRandRange(-AngularSpread, AngularSpread) / 180.f;
		const float VerticalRandom = FMath::FRandRange(-AngularSpread, AngularSpread) / 180.f;
		const FVector TraceXY = FMath::Lerp(TraceOrigin->GetForwardVector(), TraceOrigin->GetRightVector(), HorizontalRandom);
		const FVector TraceZ = FMath::Lerp(TraceOrigin->GetForwardVector(), TraceOrigin->GetUpVector(), VerticalRandom);

		const FVector TraceStart = TraceOrigin->GetComponentLocation();
		const FVector TraceEnd = TraceStart + ((TraceXY + TraceZ).GetSafeNormal() * MaxFireRange);

		const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, Params);

		AActor* const HitActor = Hit.GetActor();

		if (bHit && HitActor != nullptr)
		{
			AActor* const OwningActor = TraceOrigin->GetOwner();
			HitActor->TakeDamage(DamagePerBullet, FDamageEvent(), OwningActor->GetInstigatorController(), OwningActor);
		}

		ReceiveProjectileFired(Mesh->GetSocketLocation(BarrelSocketName), bHit ? Hit.ImpactPoint : TraceEnd);

#if WITH_EDITOR
		if (bDrawFireTraces)
		{
			DrawDebugLine(GetWorld(), Mesh->GetSocketLocation(BarrelSocketName), bHit ? Hit.ImpactPoint : TraceEnd, FColor::Red, false, 3.f);
		}
#endif
	}

	else
	{
		/** Rather than report a hit event every time an actor is struck by an individual bullet
		 *  We are going to sum the number of bullets that hit a given actor and report the damage
		 *  event after that occurs. This is signifcantly more performant because there is a lot
		 *  of pipeline stuff that occurs when reporting a damage event. Additionally, this means
		 *  Blueprint events that do things like play sound will only be called once instead many times
		 */
		TArray<AActor*> Actors;
		TArray<int32> Hits;

		for (int32 i = 0; i < BulletsPerFire; ++i)
		{
			FHitResult Hit;

			const float HorizontalRandom = FMath::FRandRange(-AngularSpread, AngularSpread) / 180.f;
			const float VerticalRandom = FMath::FRandRange(-AngularSpread, AngularSpread) / 180.f;
			const FVector TraceXY = FMath::Lerp(TraceOrigin->GetForwardVector(), TraceOrigin->GetRightVector(), HorizontalRandom);
			const FVector TraceZ = FMath::Lerp(TraceOrigin->GetForwardVector(), TraceOrigin->GetUpVector(), VerticalRandom);

			const FVector TraceStart = TraceOrigin->GetComponentLocation();
			const FVector TraceEnd = TraceStart + ((TraceXY + TraceZ).GetSafeNormal() * MaxFireRange);

			const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, Params);

			AActor* const HitActor = Hit.GetActor();

			if (bHit && HitActor != nullptr)
			{
				//Although this operation is O(n), this array is likely to only ever hit 4/5 enemies max.
				//We realistically don't have to worry about perfomance implications in that regard
				int32 HitIndex = Actors.IndexOfByKey(HitActor);

				if (HitIndex != INDEX_NONE)
				{
					++Hits[HitIndex];
				}

				//We have not hit this actor before, add it
				else
				{
					Actors.Add(HitActor);
					Hits.Add(1);
				}
			}

			ReceiveProjectileFired(Mesh->GetSocketLocation(BarrelSocketName), bHit ? Hit.ImpactPoint : TraceEnd);

#if WITH_EDITOR
			if (bDrawFireTraces)
			{
				DrawDebugLine(GetWorld(), Mesh->GetSocketLocation(BarrelSocketName), bHit ? Hit.ImpactPoint : TraceEnd, FColor::Red, false, 3.f);
			}
#endif
		}

		AActor* const OwningActor = TraceOrigin->GetOwner();

		//Ok now lets sum the damage from each bullet and deal the damage
		for (int32 i = 0; i < Actors.Num(); ++i)
		{
			float Damage = Hits[i] * DamagePerBullet;
			Actors[i]->TakeDamage(Damage, FDamageEvent(), OwningActor->GetInstigatorController(), OwningActor);
		}
	}
}

void AGun::AddRecoilVelocity()
{
	const float Multiplier = bIsAimed ? RecoilAimFactor : 1.f;
	ActiveRecoil += Recoil * Multiplier;
	ActiveRecoil = FMath::Min(ActiveRecoil, RecoilLimit);
}

void AGun::AddImpulseVelocity()
{
	AnimData.ImpulseVelocity += AnimData.Impulse;
	AnimData.ImpulseVelocity = FMath::Min(AnimData.ImpulseVelocity, AnimData.ImpulseVelocityMax);
}

void AGun::NotifyHUD()
{
	HUDData.LoadedAmmo = LoadedAmmo;
	HUDData.ExcessAmmo = ExcessAmmo;
	HUDData.MaxAmmo = MaxAmmo;

	Super::NotifyHUD();
}