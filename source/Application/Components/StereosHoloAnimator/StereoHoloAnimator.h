//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2017
//

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "Application/SlideReader/SlideReader.h"

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

/// Custom logic component for slide transition animations
class StereoHoloAnimatior : public LogicComponent {
    URHO3D_OBJECT(StereoHoloAnimatior, LogicComponent);

   private:
    /// Forward movement speed.
    float moveSpeed_;
    /// Rotation speed.
    float rotationSpeed_;
    // this is a variable that keeps track of the ongoing rotation per slide
    // swap
    float rotationProgress;

    // mod pitch and yaw are references from the application, so we can move the
    // camera for the user
    float *refPitch_;
    float *refYaw_;
    float *refRadius;

    void (*cameraUpdateCallback)();
    void (*callbackAfterExec)();
    slideElement slideElement_;
    int animationEnded;  // tells if the animation is concluded

   public:
    /// Construct.
    StereoHoloAnimatior(Context *context);

    /// Set motion parameters: forward movement speed, rotation speed, and
    /// movement boundaries.
    void SetParameters(slideElement se, float *pPitch, float *pYaw,
                       float *pRadius, float moveSpeed, float rotateSpeed);
    /// Handle scene update. Called by LogicComponent base class.
    virtual void Update(float timeStep) override;

    /// Return forward movement speed.
    float GetMoveSpeed() const { return moveSpeed_; }
    /// Return rotation speed.
    float GetRotationSpeed() const { return rotationSpeed_; }

    void setCameraUpdateCallback(void (*f)());
    void setCallbackAfterExec(void (*f)());
};  //
#endif