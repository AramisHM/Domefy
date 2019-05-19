/*
 * File: VHP.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 18th May 2019 9:54:43 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 18th May 2019 9:57:32 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef VHP_H
#define VHP_H

#include <CoreParameters.h>
#include <Urho3D.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3DAll.h>
#include <vector2.h>
#include <vector3.h>
#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

/// Custom logic component: Used to create the Visible Human
class VHP : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(VHP, LogicComponent);

   private:
   public:
    VHP(Context *context);
    ~VHP();

    // Override
    virtual void Update(float timeStep) override;
};

#endif
