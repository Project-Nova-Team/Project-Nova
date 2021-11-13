#include "GeneratorPiece.h"
#include "HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Player/Shooter.h"

AGeneratorPiece::AGeneratorPiece()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName("Pawn");
}

void AGeneratorPiece::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AGeneratorPiece::OnOverlap);
}

void AGeneratorPiece::OnOverlap(AActor* OvelappedActor, AActor* OtherActor)
{
	//Introducing this hard dependency because realistically nothing else is ever going to repair the generator
	//This can be better adapted by first correcting the crappy melee component to be attached to a weapon instead of a pawn
	//This would make the depedency tied to a weapon class instead which is far more appropriate
	if (!bIsRepaired && OtherActor->IsA(AShooter::StaticClass()))
	{
		bIsRepaired = true;
		OnGeneratorPieceRepair.Broadcast();
		Mesh->SetStaticMesh(FixedMesh);
	}
}