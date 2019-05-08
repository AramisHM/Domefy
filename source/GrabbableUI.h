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

#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss
#include "CoreParameters.h"
#include "vector2.h"
#include "vector3.h"

#include "Urho3D.h"
#include "Urho3DAll.h"

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<GrabbableUI>(); to do so

/// Custom logic component: Used to
class GrabbableUI : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(GrabbableUI, LogicComponent);

   private:
    /// Forward movement speed.
    fpmed::Vec2<float> momentum;
    fpmed::Vec2<float> coords;
    float radius;

    // Callback function pointers
    void (*updateCallback)();
    void (*callbackAfterExec)();

    bool animationEnded;  // tells if the animation is concluded
    Node *orbitableNode;

   public:
    /// Construct.
    GrabbableUI(Context *context);
    ~GrabbableUI();

    // Set the grabbable momentum

    void SetCoordinates(fpmed::Vec2<float>);
    void SetMomentum(fpmed::Vec2<float>);
    void SetRadius(float r);
    void SetOrbitableNode(Node *n);

    Vec2<float> GetMomentum();
    Vec2<float> GetCoordinates();
    float GetRadius();

    // Runs every render loop
    virtual void Update(float timeStep) override;

    // Callback templates that can be used as point to function to be executed
    // after Updates

    void SetUpdateCallback(void (*f)());
    void SetCallbackAfterExec(void (*f)());
};

#endif
