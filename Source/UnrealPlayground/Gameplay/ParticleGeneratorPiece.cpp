#include "ParticleGeneratorPiece.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Player/Shooter.h"

AParticleGeneratorPiece::AParticleGeneratorPiece()
{
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("Particle System");
	ParticleSystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AParticleGeneratorPiece::BreakPiece()
{
	AGeneratorPiece::BreakPiece();
}