//Fill out your copyright notice in the Description page of Project Settings.

#include "VaultRaycasterComponent.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UVaultRaycasterComponent::UVaultRaycasterComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}


// Called when the game starts
void UVaultRaycasterComponent::BeginPlay()
{
    Super::BeginPlay();

    Shooter = Cast<AShooter>(GetOwner());

    Cam = Shooter->GetCamera();
}

void UVaultRaycasterComponent::CheckForVault()
{

}

void UVaultRaycasterComponent::CastRay()
{

}

void UVaultRaycasterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    //Hit contains information about what the raycast hit.
    FHitResult Hit;

    //The Origin of the raycast
    StartLocation = Shooter->GetActorLocation() - FVector(0,0,70);

    ParentDirectionVector = Cam->GetComponentRotation().Vector();

    ParentDirectionVector.Z *= 0;

    //The EndLocation of the raycast
    EndLocation = StartLocation + (ParentDirectionVector * RayLength);

    //Collision parameters. The following syntax means that we don't want the trace to be complex
    FCollisionQueryParams CollisionParameters;

    CollisionParameters.AddIgnoredActor(Shooter);

    //Perform the line trace
    //The ECollisionChannel parameter is used in order to determine what we are looking for when performing the raycast
    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, CollisionParameters);

    //DrawDebugLine is used in order to see the raycast we performed
    //The boolean parameter used here means that we want the lines to be persistent so we can see the actual raycast
    //The last parameter is the width of the lines.
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true);

    if(Hit.Actor != nullptr)
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Hit.ToString());
    
}
