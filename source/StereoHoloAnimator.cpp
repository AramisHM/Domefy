#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Scene/Scene.h>

#include "StereoHoloAnimator.h"

#include <Urho3D/DebugNew.h>

#include <stdio.h>

StereoHoloAnimatior::StereoHoloAnimatior(Context* context) :
	LogicComponent(context),
	moveSpeed_(0.0f),
	rotationSpeed_(0.0f)
{
	// Only the scene update event is needed: unsubscribe from the rest for optimization
	SetUpdateEventMask(USE_UPDATE);
	rotationProgress = 0.0f;
	animationEnded = 1;
}

void StereoHoloAnimatior::SetParameters(slideElement se, float *pPitch, float *pYaw, float *pRadius, float moveSpeed, float rotationSpeed)
{
	refPitch_ = pPitch;
	refYaw_ = pYaw;
	refRadius = pRadius;
	animationEnded = 0;
	if(rotationProgress == 0) {
		moveSpeed_ = moveSpeed;
		rotationSpeed_ = rotationSpeed;
		rotationProgress = 360.0f;
		slideElement_ = se;

		//(*pPitch) = slideElement_.coord.getX();
		//(*pYaw) = slideElement_.coord.getY();
	}
}

void StereoHoloAnimatior::Update(float timeStep)
{
	//node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);
	StaticModel* hologramPlane;
	hologramPlane =  node_->GetComponent<StaticModel>();
	Material *mymat = hologramPlane->GetMaterial();

	float step = (rotationSpeed_ * timeStep);

	if(rotationProgress > 0) {
		cameraUpdateCallback();
		if((rotationProgress - abs(step)) < 0) {
			if(step>0)
				node_->Yaw(rotationProgress);
			else
				node_->Yaw(-rotationProgress);
			rotationProgress = 0;
		} else {
			node_->Yaw(step);
			rotationProgress-=abs(step);
		}
	}

	if(slideElement_.coord.getX() == -1 && slideElement_.coord.getY() == -1)
		animationEnded = 1;

	if(!animationEnded) {

		float limit = 2.0f;

		if(abs(slideElement_.coord.getX() - (*refPitch_)) > limit) {
			if(slideElement_.coord.getX() > (*refPitch_))
				 ++(*refPitch_);
			else
				 --(*refPitch_);
		}
		

		if(abs(slideElement_.coord.getY() - (*refYaw_)) > limit) {
			if(slideElement_.coord.getY() > (*refYaw_))
				 ++(*refYaw_);
			else
				 --(*refYaw_);
		}

		if(abs(slideElement_.coord.getZ() - (*refRadius)) > limit) {
			if(slideElement_.coord.getZ() > (*refRadius))
				 ++(*refRadius);
			else
				 --(*refRadius);
		}


		cameraUpdateCallback();

		if(abs(slideElement_.coord.getX() - (*refPitch_)) < limit &&
			abs(slideElement_.coord.getY() - (*refYaw_)) < limit &&
			abs(slideElement_.coord.getZ() - (*refRadius)))
			animationEnded = 1;
	}
	
}

void StereoHoloAnimatior::setCameraUpdateCallback(void (*f)()) {
	cameraUpdateCallback = f;
}

void StereoHoloAnimatior::setCallbackAfterExec(void (*f)()) {
	callbackAfterExec = f;
}