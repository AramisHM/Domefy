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

// This file contains the implementation for the grabbable UI interface for the
// fulldome platform

#ifndef GRABBABLEUI_H
#define GRABBABLEUI_H

#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss
#include "CoreParameters.h"
#include "vector3.h"

#include "vector2.h"

#include "Urho3D.h"
#include "Urho3DAll.h"

//#include <Urho3D/Math/Vector2.h>

namespace fpmed {

class GrabbableUI {
   private:
    Urho3D::Node* _node;  // the scene node that holds the Urho3D entity
    float _xaccel;        // used to give inertia effect
    float _yaccel;

    // coords X Y
    float _coordX;
    float _corodY;
    float _radius;

   public:
    GrabbableUI();
    ~GrabbableUI();
    Urho3D::Node* GetSceneNode();
    void SetSceneNode(Urho3D::Node* n);
};

}  // namespace fpmed

#endif