#include "GeneratorPiece.h"

void AGeneratorPiece::InteractionEvent(APawn* EventSender)
{
	// Don't need this. These if statements were used for testing
	if (MeshComponent->CustomDepthStencilValue == 0)
		ShowHighlightedOutline();
	else
		MeshComponent->SetCustomDepthStencilValue(0);
}

// Called when the game starts or when spawned
void AGeneratorPiece::BeginPlay()
{
	Super::BeginPlay();

	Trigger = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));

	MeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGeneratorPiece::BeginOverlap);

	bIsInteractable = false;
}

void AGeneratorPiece::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->IsA(UMeleeComponent::StaticClass()))
	{
		OnGeneratorPieceHit.Broadcast();

		SwapMeshToFixed();
	}
}


void AGeneratorPiece::SwapMeshToFixed()
{
	// Doesn't inherit scale and rotation properties of original mesh. May need to fine tune each mesh to make sure it looks right
	Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetStaticMesh(FixedMesh);
}

void AGeneratorPiece::ShowHighlightedOutline()
{
	MeshComponent->SetCustomDepthStencilValue(2);
}
