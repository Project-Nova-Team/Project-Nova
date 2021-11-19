#include "ParticleGeneratorPiece.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Player/Shooter.h"

AParticleGeneratorPiece::AParticleGeneratorPiece()
{
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("Particle System");
	ParticleSystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AParticleGeneratorPiece::BeginPlay()
{
	Super::BeginPlay();
}

void AParticleGeneratorPiece::BreakPiece()
{
	AGeneratorPiece::BreakPiece();
}

void AParticleGeneratorPiece::OnOverlap(AActor* OvelappedActor, AActor* OtherActor)
{
	if (!bIsRepaired && OtherActor->IsA(AShooter::StaticClass()))
	{
		bIsRepaired = true;
		OnGeneratorPieceRepair.Broadcast();
		Mesh->SetStaticMesh(FixedMesh);
		ParticleSystem->DeactivateSystem();
	}
}