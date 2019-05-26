/*
 * File: AnatomicViewer.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 25th May 2019 1:43:28 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 25th May 2019 1:43:46 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef ANATOMICVIEWER_H
#define ANATOMICVIEWER_H

#include <Core/CoreParameters.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <Urho3D.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3DAll.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>  // c++ standar vector calss

#include <Application/Components/GrabbableUI/GrabbableUI.h>

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

/// Custom logic component: Used to create the Visible Human
class AnatomicViewer : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(AnatomicViewer, LogicComponent);

   private:
    Node* viewerNode;  // The reference node that orbits arround the root node
    Node* viewerModelNode;    // the reference to the model node
    GrabbableUI* viewerGrab;  // the grabbable ui componenet to move the viewer
                              // arround the camera

   public:
    AnatomicViewer(Context* context);
    ~AnatomicViewer();

    // CreateViewer - Creates the viewer model into the root node.
    void CreateViewer();

    // Override
    virtual void Update(float timeStep) override;
};

#endif