/*
 * File: GrabbableUI.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 4th May 2019 12:58:36 am
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 4th May 2019 1:04:50 am
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

// This file contains the implementation for the grabbable UI interface for the
// fulldome platform

#include "GrabbableUI.h"

// constructor and destructor
GrabbableUI::GrabbableUI(Urho3D::Context *context)
    : Urho3D::LogicComponent(context) {
    // Only the scene update event is needed: unsubscribe from the rest for
    // optimization
    SetUpdateEventMask(USE_UPDATE);
    animationEnded = 1;
    this->orbitableNode = 0;
}
GrabbableUI::~GrabbableUI() {}

void GrabbableUI::SetCoordinates(Vec2<float> c) { this->coords = c; }
void GrabbableUI::SetMomentum(Vec2<float> m) { this->momentum = m; }

void GrabbableUI::Update(float timeStep) {
    node_->Rotate(Quaternion(
        timeStep *
        0.02f));  // do some stuff with the node that has the  componenet
    // node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);
    // updateCallback();

    printf("%f\n", timeStep);
}

// Callback functions that might come handy
void GrabbableUI::SetUpdateCallback(void (*f)()) { updateCallback = f; }
void GrabbableUI::SetCallbackAfterExec(void (*f)()) { callbackAfterExec = f; }