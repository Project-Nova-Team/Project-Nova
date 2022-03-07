#include "FlameThrower.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

AFlameThrower::AFlameThrower()
{
	FlameSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Flame System"));
	FlameSystemComponent->SetupAttachment(Mesh, BarrelSocketName);

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
		FlameSystemComponent->ActivateSystem(true);
		FlameAudio->Play();
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
		SparkAudioComponent->Stop();
		SparkSystemComponent->DeactivateSystem();
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
	FlameSystemComponent->ActivateSystem();
	FlameAudio->Play();
	SparkSystemComponent = UGameplayStatics::SpawnEmitterAttached(SparkSystem, Mesh, BarrelSocketName);
	SparkAudioComponent = UGameplayStatics::SpawnSoundAttached(SparkAudio, Mesh, BarrelSocketName);
}

void AFlameThrower::StopFlamethrower()
{
	if (SparkAudioComponent)
	{
		SparkSystemComponent->DeactivateSystem();
	}

	if (SparkAudioComponent)
	{
		SparkAudioComponent->Stop();
	}

	FlameSystemComponent->DeactivateSystem();
	FlameAudio->Stop();
}