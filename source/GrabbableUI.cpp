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

namespace fpmed {

// constructor and destructor
GrabbableUI::GrabbableUI() { this->_node = 0; }
GrabbableUI::~GrabbableUI() {}

Urho3D::Node* GrabbableUI::GetSceneNode() { return this->_node; }
void GrabbableUI::SetSceneNode(Urho3D::Node* n) { this->_node = n; }

}  // namespace fpmed