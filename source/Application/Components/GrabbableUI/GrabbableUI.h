/*
 * File: GrabbableUI.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 6th May 2019 23:02:00 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 6th May 2019 23:02:00 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef GRABBABLEUI_H
#define GRABBABLEUI_H

#include <Core/CoreParameters.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss

#include <Urho3D.h>
#include <Urho3DAll.h>

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<GrabbableUI>(); to do soc

/// Custom logic component: Used to
class GrabbableUI : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(GrabbableUI, LogicComponent);

   private:
    /// Forward movement speed.
    fpmed::Vec2<float> momentum;
    fpmed::Vec2<float> coords;
    float radius_;
    float momentumTriggerVal;
    float MOUSE_SENSITIVITY = 0.2f;  // make methodes to get and set

    // Callback function pointers
    void (*updateCallback)();
    void (*callbackAfterExec)();
    // calculates the momentum for each frame rendered
    void UpdateMomentum(float timeStep);

    bool animationEnded;  // tells if the animation is concluded
    Node *orbitableNode;
    Vector3 rotationOffset;  // always apply it to the model when calculating
                             // its transformations

   public:
    /// Construct.
    GrabbableUI(Context *context);
    ~GrabbableUI();

    // Set the grabbable momentum

    void SetCoordinates(fpmed::Vec2<float>);
    void SetMomentum(fpmed::Vec2<float>);
    void SetRadius(float r);
    void SetOrbitableNode(Node *n);
    void MoveArroundOrbitableNode(float yaw, float pitch, float radius,
                                  Urho3D::Vector3 correction);
    void MoveArroundOrbitableReference(float yaw, float pitch, float radius,
                                       Urho3D::Vector3 reference,
                                       Urho3D::Vector3 correction);
    // Given a mouse movement, automatically apply the movement to the root
    // node.
    void ApplyMouseMove(Vec2<int> move);
    Vec2<float> GetMomentum();
    Vec2<float> GetCoordinates();
    float GetRadius();

    // Runs every render loop
    virtual void Update(float timeStep) override;

    // Callback templates that can be used as point to function to be executed
    // after Updates

    void SetUpdateCallback(void (*f)());
    void SetCallbackAfterExec(void (*f)());

    void SetRotationOffset(Vector3 v);
};

#endif
