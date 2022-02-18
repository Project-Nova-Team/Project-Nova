#include "GeneratorPiece.h"
#include "HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Weapon/MeleeWeapon.h"

AGeneratorPiece::AGeneratorPiece()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName("Pawn");
}

void AGeneratorPiece::BeginPlay()
{
	Super::BeginPlay();
	BrokenMesh = Mesh->GetStaticMesh();
	OnActorBeginOverlap.AddDynamic(this, &AGeneratorPiece::OnOverlap);
}

void AGeneratorPiece::OnOverlap(AActor* OvelappedActor, AActor* OtherActor)
{
	if (!bIsRepaired && OtherActor->IsA(AMeleeWeapon::StaticClass()))
	{
		bIsRepaired = true;
		OnGeneratorPieceRepair.Broadcast();
		Mesh->SetStaticMesh(FixedMesh);
	}
}

void AGeneratorPiece::BreakPiece()
{
	if (bIsRepaired)
	{
		bIsRepaired = false;
		Mesh->SetStaticMesh(BrokenMesh);
	}
}