#pragma once

#include "CoreMinimal.h"
#include "SMovementState.h"
#include "STuckState.generated.h"

/** Tuck states handle the resizing and repositioning of the player collider and camera respectively
 *	On exit they initiate a timer to stand back from from a crouch or prone state
 */
UCLASS()
class PROJECTNOVA_API USTuckState : public USMovementState
{
	GENERATED_BODY()

public:
	USTuckState() { }
	~USTuckState() { }

	virtual void OnEnter() override;
	virtual void OnExit() override;

protected:

	/** The half height of the collider this state will try to maintain*/
	float TargetHalfHeight;

	/** The relative z position of the camera joint this state will try to maintain*/
	float TargetCameraHeight;

	/** 
	* Begins timer that adjusts the collider height and camera position over time
	* 
	* @param	NewColliderHalfHeight	The half height of the collider that will be interpolated to
	* @param	ExpectedHeight			The half height of the collider this state tries to maintain
	* @param	NewCameraHeight			The relative height the camera will be interpolated to
	*/
	void StartHeightInterpolation(const float NewColliderHalfHeight, const float ExpectedHeight, const float NewCameraHeight);

	/** Adjusts the Shooter collider height and camera position over time to a standing position
	* 
	 * @param	StartHeight				The half height of the collider at the beginning of the transition
	 * @param	TargetHeight			The half height of the collider at the end of the transition
	 * @param	CameraStartHeight		The relative camera joint Z position at the beginning of the transition
	 * @param	TargetCamHeight			The relative camera joint Z position at the end of the transition
	 */
	void AdjustHeight(const float StartHeight, const float TargetHeight, const float CameraStartHeight, const float TargetCamHeight);

	/** 
	* Evaluates if the new collider height would collide with terrain that would make it impossible to stand
	* 
	* @param	NewColliderHalfHeight	The half height of the collider that will be interpolated to
	* @returns	True if a capsule sweep at the specified half height does not collide with level geometry
	*/
	bool CheckIfStandUpIsValid(const float NewColliderHalfHeight) const;

	FDelayedActionHandle* Handle;
};
