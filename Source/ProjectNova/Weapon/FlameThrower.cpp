#include "FlameThrower.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

AFlameThrower::AFlameThrower()
{
	FlameAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Flame Audio"));
	FlameAudio->SetupAttachment(Mesh, BarrelSocketName);

	MaxFireRange = 75.f;

	bAttackContinuously = true;
}

void AFlameThrower::Attack()
{
	//If this is the first request to attack, do some set up
	if (!bAttacking)
	{
		FlameAudio->Play();
		FlameSystemComponent = UGameplayStatics::SpawnEmitterAttached(FlameSystem, Mesh, BarrelSocketName);
		bAttacking = true;
	}

	//Check if the flame is hitting anything
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TraceOrigin->GetOwner());
	
	FTransform SocketTransform = Mesh->GetSocketTransform(BarrelSocketName);

	const FVector Forward = FRotationMatrix(SocketTransform.Rotator()).GetUnitAxis(EAxis::X);
	const FVector Start = Mesh->GetSocketLocation(BarrelSocketName);

#if WITH_EDITOR
	if (bDrawFireTraces)
	{
		DrawDebugLine(GetWorld(), Start, Start + (Forward * MaxFireRange), FColor::Red, false, 4.f);
	}
#endif
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, Start + (Forward * MaxFireRange), ECC_Pawn, Params);

	if (bHit && !bHitLastFrame)
	{
		SparkSystemComponent = UGameplayStatics::SpawnEmitterAttached(SparkSystem, Mesh, BarrelSocketName);
		SparkAudioComponent = UGameplayStatics::SpawnSoundAttached(SparkAudio, Mesh, BarrelSocketName);
	}

	else if (!bHit && bHitLastFrame)
	{
		SparkAudioComponent.Get()->Stop();
		SparkSystemComponent.Get()->DeactivateSystem();
	}

	bHitLastFrame = bHit;
}

void AFlameThrower::StopAttack()
{
	StopFlamethrower();
	
	bHitLastFrame = false;
	bAttacking = false;
}

void AFlameThrower::StartFlamethrower()
{
	FlameAudio->Play();
	FlameSystemComponent = UGameplayStatics::SpawnEmitterAttached(FlameSystem, Mesh, BarrelSocketName);
	SparkSystemComponent = UGameplayStatics::SpawnEmitterAttached(SparkSystem, Mesh, BarrelSocketName);
	SparkAudioComponent = UGameplayStatics::SpawnSoundAttached(SparkAudio, Mesh, BarrelSocketName);
}

void AFlameThrower::StopFlamethrower()
{
	if (SparkAudioComponent.IsValid())
	{
		SparkSystemComponent.Get()->DeactivateSystem();
	}

	if (SparkAudioComponent.IsValid())
	{
		SparkAudioComponent.Get()->Stop();
	}

	if (FlameSystemComponent.IsValid())
	{
		FlameSystemComponent.Get()->DeactivateSystem();
	}
	
	FlameAudio->Stop();
}