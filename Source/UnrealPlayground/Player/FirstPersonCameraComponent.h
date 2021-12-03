#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "FirstPersonCameraComponent.generated.h"

UENUM()
enum ECameraSwayState
{
	Sway_None,
	Sway_Walk,
	Sway_Run
};

UCLASS()
class UNREALPLAYGROUND_API UFirstPersonCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UFirstPersonCameraComponent();

	UFUNCTION(BlueprintCallable)
	void ProcessCameraSway();

	UFUNCTION(BlueprintCallable)
	void SetState(const ECameraSwayState NewState);

protected:
	/** Live state of the camera which determines amount of sway*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Camera Sway | State")
	TEnumAsByte<ECameraSwayState> State;

	/** Max amount of vertical offset when walking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | State")
	float BlendTime;

	/** Max amount of vertical offset when walking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Walking")
	float WalkSwayAmplitude;

	/** Speed at which vertical offset occurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Walking")
	float WalkSwayFrequency;

	/** Max amount of side to side rotation of the camera when walking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Walking")
	float WalkRotationAmplitude;

	/** Speed at which side to side rotation occurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Walking")
	float WalkRotationFrequency;

	/** Max amount of vertical offset when walking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Running")
	float RunSwayAmplitude;

	/** Speed at which vertical offset occurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Running")
	float RunSwayFrequency;

	/** Max amount of side to side rotation of the camera when walking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Running")
	float RunRotationAmplitude;

	/** Speed at which side to side rotation occurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sway | Running")
	float RunRotationFrequency;

private:

	float timer;

	bool bIsBlending;

	/// Below properties are used in actual calculation of camera positional/rotational offset and are updated when state changes ///

	float ActiveSwayAmplitude;

	float ActiveSwayFrequency;

	float ActiveRotationAmplitude;

	float ActiveRotationFrequency;

	// Initial Blends

	float PreBlendSway;

	float PreBlendRotation;
	
};
