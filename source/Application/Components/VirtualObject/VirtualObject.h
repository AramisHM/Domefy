/*
 * File: VirtualObject.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 25th May 2019 2:35:37 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 25th May 2019 2:37:18 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef VIRTUALOBJECTCOMPONENT_H
#define VIRTUALOBJECTCOMPONENT_H

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

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

#define MAX_NUM_SLICES 7000

/// Custom logic component: Used to create the virtual "hologram"
class VirtualObject : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(VirtualObject, LogicComponent);

   private:
   public:
    VirtualObject(Context* context);
    ~VirtualObject();
    // Creates the model from Visible Human Project
    void CreateModel();

    // Override
    virtual void Update(float timeStep) override;
};

#endif