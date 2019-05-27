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

void GrabbableUI::UpdateMomentum(float timeStep) {
    if (momentum.getX() || momentum.getY()) {
        // decay the momentum - TODO: make quadratic instead of linear
        float decayFactor = 0.5f;
        float factor = decayFactor * timeStep;
        if (momentum.getX() > 0) {
            float x = momentum.getX();
            x = x - factor < 0 ? 0 : x - factor;
            momentum.setX(x);
        }
        if (momentum.getX() < 0) {
            float x = momentum.getX();
            x = x + factor > 0 ? 0 : x + factor;
            momentum.setX(x);
        }
        if (momentum.getY() > 0) {
            float y = momentum.getY();
            y = y - factor < 0 ? 0 : y - factor;
            momentum.setY(y);
        }
        if (momentum.getY() < 0) {
            float y = momentum.getY();
            y = y + factor > 0 ? 0 : y + factor;
            momentum.setY(y);
        }
    }
    // Apply the momentum to the node
    coords += momentum;  // sum momentum to the coords
    coords.setY(Clamp(coords.getY(), -90.0f, 90.0f));
    MoveArroundOrbitableReference(coords.getX(), coords.getY(), radius_,
                                  Vector3(0, 0, 0), Vector3(90, -90, 0));
}

// Move a node arround according to a sphere space system
void GrabbableUI::MoveArroundOrbitableNode(
    float yaw, float pitch, float radius,
    Urho3D::Vector3 correction = Vector3(0.0f, -90.0f, 0.0f)) {
    coords = Vec2<float>(yaw, pitch);
    radius_ = radius;
    if (orbitableNode) {
        float radiusMagnetude = Cos(Abs(pitch));
        Vector3 oldpos;
        if (node_ == orbitableNode) {  // linked reference
            oldpos = Vector3(.0f, .0f, .0f);
        } else {
            oldpos = orbitableNode->GetPosition();
        }

        node_->SetPosition(
            Vector3((Cos(yaw) * radius * radiusMagnetude) + oldpos.x_,
                    (Sin(pitch) * radius) + oldpos.y_,
                    (-Sin(yaw) * radius * radiusMagnetude) + oldpos.z_));

        // Construct new orientation for the camera scene node from yaw and
        // pitch. Roll is fixed to zero
        node_->SetRotation(Quaternion(pitch + correction.x_,
                                      yaw + correction.y_, correction.z_));
    }
}

void GrabbableUI::MoveArroundOrbitableReference(float yaw, float pitch,
                                                float radius,
                                                Urho3D::Vector3 reference,
                                                Urho3D::Vector3 correction) {
    coords = Vec2<float>(yaw, pitch);
    radius_ = radius;
    if (orbitableNode) {
        float radiusMagnetude = Cos(Abs(pitch));
        node_->SetPosition(
            Vector3((Cos(yaw) * radius * radiusMagnetude) + reference.x_,
                    (Sin(pitch) * radius) + reference.y_,
                    (-Sin(yaw) * radius * radiusMagnetude) + reference.z_));

        // Construct new orientation for the camera scene node from yaw and
        // pitch. Roll is fixed to zero
        node_->SetRotation(Quaternion(pitch + correction.x_,
                                      yaw + correction.y_, correction.z_));
    }
}

void GrabbableUI::ApplyMouseMove(Vec2<int> mouseDelta) {
    // Moves slide arround camera
    // Get mouse move
    // Apply momentum
    // Calculate movement and clamp
    // apply the movement to the current coords

    Vec2<float> slideCoords = GetCoordinates();
    Vec2<float> move;

    move.setX(MOUSE_SENSITIVITY / 3.1415926f * (radius_ / 30) *
              mouseDelta.getX());
    move.setY(-MOUSE_SENSITIVITY / 3.1415926f * (radius_ / 30) *
              mouseDelta.getY());
    move.setY(Clamp(move.getY(), -90.0f, 90.0f));

    slideCoords += move;

    MoveArroundOrbitableReference(slideCoords.getX(), slideCoords.getY(),
                                  radius_, Vector3(0, 0, 0),
                                  Vector3(90, -90, 0));
}

// constructor and destructor
GrabbableUI::GrabbableUI(Urho3D::Context* context)
    : Urho3D::LogicComponent(context) {
    // Only the scene update event is needed: unsubscribe from the rest for
    // optimization
    SetUpdateEventMask(USE_UPDATE);
    animationEnded = 1;
    this->orbitableNode = 0;  // ground
    momentumTriggerVal = 1.0f;
    this->coords = Vec2<float>(260, 0);
    radius_ = 25.0f;
    MOUSE_SENSITIVITY = 0.2f;
}
GrabbableUI::~GrabbableUI() {}

Vec2<float> GrabbableUI::GetCoordinates() { return coords; }
void GrabbableUI::SetCoordinates(Vec2<float> c) { this->coords = c; }
void GrabbableUI::SetMomentum(Vec2<float> m) { this->momentum = m; }
void GrabbableUI::SetOrbitableNode(Node* n) { orbitableNode = n; }

void GrabbableUI::Update(float timeStep) {
    // node_->Rotate(Quaternion(
    //     timeStep *
    //     0.02f));  // do some stuff with the node that has the  componenet

    if (orbitableNode != NULL) {
        // simulate the momentum if any
        // MoveArroundOrbitableNode();
        UpdateMomentum(timeStep);

        if (radius_ < 0.01f) radius_ = 0.01f;
        if (radius_ > 30.0f) radius_ = 30.f;
    }
    // node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);
    // updateCallback();

    // printf("%f\n", timeStep);
}

// Callback functions that might come handy
void GrabbableUI::SetUpdateCallback(void (*f)()) { updateCallback = f; }
void GrabbableUI::SetCallbackAfterExec(void (*f)()) { callbackAfterExec = f; }

void GrabbableUI::SetRadius(float r) { radius_ = r; }
float GrabbableUI::GetRadius() { return radius_; }