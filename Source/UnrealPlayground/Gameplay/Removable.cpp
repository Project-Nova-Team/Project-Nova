#include "Removable.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

// Add default functionality here for any IRemovable functions that are not pure virtual.
void IRemovable::RemoveSelf(AActor* Actor)
{
	TArray<UStaticMeshComponent*> Components;
	Actor->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		Components[i]->SetVisibility(false);
		Components[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void IRemovable::RestoreSelf(AActor* Actor)
{
	TArray<UStaticMeshComponent*> Components;
	Actor->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		Components[i]->SetVisibility(true);
		Components[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
