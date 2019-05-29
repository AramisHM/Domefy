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
                                  rotationOffset);
}

// Move a node arround according to a sphere space system
void GrabbableUI::MoveArroundOrbitableReference(
    float yaw, float pitch, float radius,
    Urho3D::Vector3 correction = Vector3(0.0f, -90.0f, 0.0f)) {
    coords = Vec2<float>(yaw, pitch);
    radius_ = radius;
    float radiusMagnetude = Cos(Abs(pitch));

    // RereferenceVector
    Vector3 oldpos;
    if (dynamicOrbitableReference == false) {
        oldpos = orbitableReference;
    } else {
        oldpos = orbitableNode->GetPosition();
    }

    node_->SetPosition(
        Vector3((Cos(yaw) * radius_ * radiusMagnetude) + oldpos.x_,
                (Sin(pitch) * radius) + oldpos.y_,
                (-Sin(yaw) * radius_ * radiusMagnetude) + oldpos.z_));

    // Construct new orientation for the camera scene node from yaw and
    // pitch. Roll is fixed to zero
    node_->SetRotation(
        Quaternion(pitch + correction.x_, yaw + correction.y_, correction.z_));
}

// void GrabbableUI::MoveArroundOrbitableReference(float yaw, float pitch,
//                                                 float radius,
//                                                 Urho3D::Vector3 reference,
//                                                 Urho3D::Vector3 correction) {
//     coords = Vec2<float>(yaw, pitch);
//     radius_ = radius;
//     if (orbitableNode) {
//         float radiusMagnetude = Cos(Abs(pitch));
//         node_->SetPosition(
//             Vector3((Cos(yaw) * radius * radiusMagnetude) + reference.x_,
//                     (Sin(pitch) * radius) + reference.y_,
//                     (-Sin(yaw) * radius * radiusMagnetude) + reference.z_));

//         // Construct new orientation for the camera scene node from yaw and
//         // pitch. Roll is fixed to zero
//         node_->SetRotation(Quaternion(pitch + correction.x_,
//                                       yaw + correction.y_, correction.z_));
//     }
// }

void GrabbableUI::ApplyMouseMove(Vec2<int> mouseDelta) {
    // Moves slide arround camera
    // Get mouse move
    // Apply momentum
    // Calculate movement and clamp
    // apply the movement to the current coords

    Vec2<float> coords = GetCoordinates();
    Vec2<float> move;

    float radiusInfluence = 1;
    if (radiusAltersMoveSpace == true) {
        radiusInfluence = (radius_ / maxDistance);
    }

    move.setX(MOUSE_SENSITIVITY / 3.1415926f * radiusInfluence *
              mouseDelta.getX());
    move.setY(-MOUSE_SENSITIVITY / 3.1415926f * radiusInfluence *
              mouseDelta.getY());
    move.setY(Clamp(move.getY(), -90.0f, 90.0f));

    coords += move;

    MoveArroundOrbitableReference(coords.getX(), coords.getY(), radius_,
                                  rotationOffset);
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
    rotationOffset = Vector3(90, -90, 0);
    dynamicOrbitableReference = false;
    radiusAltersMoveSpace = true;
    minDistance = 0.01f;
    maxDistance = 30.0f;
}
GrabbableUI::~GrabbableUI() {}

Vec2<float> GrabbableUI::GetCoordinates() { return this->coords; }
void GrabbableUI::SetCoordinates(Vec2<float> c) { this->coords = c; }
void GrabbableUI::SetMomentum(Vec2<float> m) { this->momentum = m; }
void GrabbableUI::SetOrbitableReference(Node* n) {
    orbitableNode = n;
    dynamicOrbitableReference = true;
}
void GrabbableUI::SetOrbitableReference(Vector3 v) {
    orbitableNode = NULL;
    orbitableReference = v;
    dynamicOrbitableReference = false;
}

void GrabbableUI::Update(float timeStep) {
    // node_->Rotate(Quaternion(
    //     timeStep *
    //     0.02f));  // do some stuff with the node that has the  componenet

    if (orbitableNode != NULL) {
        // simulate the momentum if any
        // MoveArroundOrbitableReference();
        UpdateMomentum(timeStep);

        if (radius_ < minDistance) radius_ = minDistance;
        if (radius_ > maxDistance) radius_ = maxDistance;
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

void GrabbableUI::SetRotationOffset(Vector3 v) { rotationOffset = v; }

void GrabbableUI::SumRadius(float r) { radius_ += r; }

void GrabbableUI::SetMinimumRadiusDistance(float r) { maxDistance = r; }

void GrabbableUI::SetMaximumRadiusDistance(float r) { minDistance = r; }

void GrabbableUI::SetRadiusAlterMoveSpeed(bool alterMoveSpeed) {
    radiusAltersMoveSpace = alterMoveSpeed;
}