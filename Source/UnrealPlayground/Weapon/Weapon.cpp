#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "CombatComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName("Ragdoll");	
	Mesh->SetSimulatePhysics(true);
	
	WeaponType = WC_Pistol;
	ThrowForce = 100000.f;

	ClipSize = 14;
	MaxHeldAmmo = 70;
	MaxFireRange = 50000.f;

	BaseDamage = 25.f;
	BodyMultiplier = 1.f;
	HeadMultiplier = 2.f;
	LimbMultiplier = 0.5f;

	FireRate = 0.5f;
	Recoil = 720.f;
	RecoilFallOff = 90.f;
	RecoilRecovery = 60.f;
	RecoilAngularLimit = 30.f;

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
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//TODO the base function still draws a fair bit of overhead.
	//Consider disabling the tick function in SetOwningComponent using PrimaryActorTick.SetTickFunctionEnable
	//to save some performance.
	if (OwningComponent == nullptr)
	{
		return;	
	}

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

void AWeapon::SetOwningComponent(UCombatComponent* NewOwner, const USceneComponent* TraceOriginComponent)
{
	//A pawn has picked the weapon up
	if (NewOwner != nullptr)
	{
		Mesh->SetVisibility(false);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Don't shoot ourselves
		QueryParams.ClearIgnoredActors();
		QueryParams.AddIgnoredActor(NewOwner->GetOwner());
		QueryParams.AddIgnoredActor(this);
	}

	//A pawn has dropped the weapon
	else
	{
		//Move the gun a little below the camera 
		const FVector DropLocation = TraceOrigin->GetComponentLocation() - FVector(0, 0, 30.f);
		SetActorLocation(DropLocation);

		Mesh->SetVisibility(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetSimulatePhysics(true);

		
		//Apply a force to make it look like the gun was thrown
		Mesh->AddForce(TraceOrigin->GetForwardVector().GetSafeNormal2D() * ThrowForce);
	}

	OwningComponent = NewOwner;
	TraceOrigin = TraceOriginComponent;
}

void AWeapon::Fire()
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
	FVector TraceDirection = TraceOrigin->GetForwardVector();

	//If weapon is not aimed, apply bloom
	const bool bIsAimed = OwningComponent->IsAimed();	
	if (!bIsAimed)
	{
		const float CurrentBloom = GetOwningComponent()->GetCurrentBloom();
		const float HorizontalRandom = FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;
		const float VerticalRandom =  FMath::FRandRange(-CurrentBloom, CurrentBloom) / 180.f;
	
		const FVector TraceXY = FMath::Lerp(TraceDirection, TraceOrigin->GetRightVector(), HorizontalRandom);
		const FVector TraceZ = FMath::Lerp(TraceDirection, TraceOrigin->GetUpVector(), VerticalRandom);	
		TraceDirection = (TraceXY + TraceZ).GetSafeNormal();

		OwningComponent->AddBloom(Bloom);
	}

	const FVector TraceEnd = TraceStart + TraceDirection * MaxFireRange;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	
	const float RecoilFactor = bIsAimed ? RecoilAimFactor : 1.f;
	OwningComponent->AddRecoilVelocity(Recoil * RecoilFactor);

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

void AWeapon::Reload()
{
	const uint16 AmmoDifference = ClipSize - CurrentAmmo;
	const uint16 AmmoToRestore = FMath::Min(AmmoDifference, ExccessAmmo);

	CurrentAmmo += AmmoToRestore;
	ExccessAmmo -= AmmoToRestore;
}