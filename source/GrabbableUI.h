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

// enum SlideType {
//     ST_TITLE = 0,         // has only a title and a sub-title
//     ST_TITLE_CONTENT = 1  // Has title, sub-title and a text/image content
// };

// typedef struct slideElement {
//     std::string materialPath;
//     // Urho3D::Vector2 coord; // camera coordinates on this slide
//     fpmed::Vec3<float> coord;  // camera coordinates on this slide
//     fpmed::Vec3<float> refPoint;
//     int interestPointIndex;  // points to the index of slide text node
//     bool hasRefPoint;
//     std::string text;
// };

namespace fpmed {

class GrabbabableUI {
   private:
    Urho3D::Node* node;  // the scene node that holds the Urho3D entity

   public:
    GrabbabableUI();
    ~GrabbabableUI();
};

}  // namespace fpmed

#endif