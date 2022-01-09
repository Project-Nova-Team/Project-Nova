#include "FirstPersonCameraComponent.h"
#include "Engine/World.h"

UFirstPersonCameraComponent::UFirstPersonCameraComponent()
{
	BlendTime = 0.1f;

	WalkRotationAmplitude = 2;
	WalkRotationFrequency = 5;
	WalkSwayAmplitude = 5;
	WalkSwayFrequency = 9;

	RunRotationAmplitude = 5;
	RunRotationFrequency = 10;
	RunSwayAmplitude = 8;
	RunSwayFrequency = 15;
}

void UFirstPersonCameraComponent::SetState(const ECameraSwayState NewState)
{
	if (NewState != State)
	{
		State = NewState;

		switch (State)
		{
		case Sway_None:
			ActiveRotationAmplitude = 0;
			ActiveRotationFrequency = 0;
			ActiveSwayAmplitude = 0;
			ActiveSwayFrequency = 0;
			break;
		case Sway_Walk:
			ActiveRotationAmplitude = WalkRotationAmplitude;
			ActiveRotationFrequency = WalkRotationFrequency;
			ActiveSwayAmplitude = WalkSwayAmplitude;
			ActiveSwayFrequency = WalkSwayFrequency;
			break;
		case Sway_Run:
			ActiveRotationAmplitude = RunRotationAmplitude;
			ActiveRotationFrequency = RunRotationFrequency;
			ActiveSwayAmplitude = RunSwayAmplitude;
			ActiveSwayFrequency = RunSwayFrequency;
			break;
		}

		timer = 0;
		bIsBlending = true;

		PreBlendRotation = GetRelativeRotation().Roll;
		PreBlendSway = GetRelativeLocation().Z;
	}
}

void UFirstPersonCameraComponent::ProcessCameraSway()
{
	timer += GetWorld()->GetDeltaSeconds();

	const FVector CurrentLocation= GetRelativeLocation();
	const FRotator CurrentRotation = GetRelativeRotation();

	float NewHeight = FMath::Sin(timer * ActiveSwayFrequency) * ActiveSwayAmplitude;
	float NewRoll= FMath::Sin(timer * ActiveRotationFrequency) * ActiveRotationAmplitude;
	
	//kind of a lazy blend but the effect is so subtle you cant really tell anyways
	if (bIsBlending)
	{
		const float BlendAlpha = FMath::Clamp(timer / BlendTime, 0.f, 1.f);

		NewHeight = FMath::Lerp(PreBlendSway, NewHeight, BlendAlpha);
		NewRoll = FMath::Lerp(PreBlendRotation, NewRoll, BlendAlpha);

		if (BlendAlpha == 1)
		{
			bIsBlending = false;
		}
	}

	const FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll);
	const FVector NewLocation = FVector(CurrentLocation.X, CurrentLocation.Y, NewHeight);
	
	SetRelativeLocationAndRotation(NewLocation, NewRotation);
}