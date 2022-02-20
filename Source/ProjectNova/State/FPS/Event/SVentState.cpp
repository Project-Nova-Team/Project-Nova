// Fill out your copyright notice in the Description page of Project Settings.

#include "SVentState.h"
#include "../../../State/FPS/ShooterStateMachine.h"
#include"../../../Gameplay/Vent.h"
#include <ProjectNova/State/FPS/Movement/SProneState.h>

void USVentState::Initialize(UStateMachine* StateMachine, UObject* ContextObject)
{
	Super::Initialize(StateMachine, ContextObject);
}

void USVentState::OnEnter()
{
	UE_LOG(LogTemp, Warning, TEXT("Vent State Enter"));
	Super::OnEnter();

	AVent* Vent = Cast<AVent>(Shooter->GetLastScannedObject());

	if (Vent)
	{
		if (Vent->GetSpline())
		{			
			Spline = Vent->GetSpline();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Spline->GetForwardVector().ToString());
	Shooter->GetAnchor()->SetWorldRotation(Spline->GetForwardVector().Rotation());

	Movement->CameraYawMaxAngle = Shooter->GetAnchor()->GetComponentRotation().Yaw;
	Movement->CameraYawMinAngle = Shooter->GetAnchor()->GetComponentRotation().Yaw;

	Movement->CameraPitchMaxAngle = Shooter->GetAnchor()->GetComponentRotation().Yaw;
	Movement->CameraPitchMinAngle = Shooter->GetAnchor()->GetComponentRotation().Yaw;

	UE_LOG(LogTemp, Warning, TEXT("max %f"), Movement->CameraYawMaxAngle);
	UE_LOG(LogTemp, Warning, TEXT("min %f"), Movement->CameraYawMinAngle);

	//Movement->CameraPitchMinAngle /= 7;
	//Movement->CameraPitchMaxAngle /= 7;
	//Movement->CameraYawMinAngle /= 2;
	//Movement->CameraYawMaxAngle /= 2;
}

void USVentState::OnExit()
{
	UE_LOG(LogTemp, Warning, TEXT("Vent State Exit"));
}

void USVentState::Tick(const float DeltaTime) 
{
	Super::Tick(DeltaTime); // change this
	Movement->bIsInTuckTransition = true;

	bClampCameraYaw = true;

	//UE_LOG(LogTemp, Warning, TEXT("%f"), Movement->CameraYawMaxAngle);
	
	Progress += Input->MoveY * 5;
	FVector Loc = FVector(Spline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World));

	Shooter->SetActorLocation(Loc);
}