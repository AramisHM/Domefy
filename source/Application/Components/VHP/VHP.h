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

/// Custom logic component: Used to create the Visible Human
class VHP : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(VHP, LogicComponent);

   private:
    unsigned int numberOfSlices;
    unsigned int heightOffset = 5;
    Material* slicesMaterials[MAX_NUM_SLICES];
    Node* slicesNodes[MAX_NUM_SLICES];

    // level of the sagital cut
    float sagitalLevel;
    float coronalLevel;
    float axialLevel;

    // dimensions
    float axialLength;
    float axialHeight;

    // Slices bases
    Node* coronalBasedDatesed;
    Node* sagitalBasedDatesed;
    Node* axialBasedDatesed;

   public:
    VHP(Context* context);
    ~VHP();
    // Creates the model from Visible Human Project
    void CreateModel();

    // Override
    virtual void Update(float timeStep) override;

    // SumSagitalCut - Adds the value from parameter to the sagital cut level
    void SumSagitalCut(float level);

    // SumCoronalCut - Adds the value from parameter to the coronal cut level
    void SumCoronal(float level);

    // SumAxialCut - Adds the value from parameter to the axial cut level
    void SumAxial(float level);

    // SetSagitalBaseVisible - Enables or disables the rendering of the sagital
    // planes.
    void SetSagitalBaseVisible(bool isVisible);
    // SetCoronalBaseVisible - Enables or disables the rendering of the coronal
    // planes.
    void SetCoronalBaseVisible(bool isVisible);
};

#endif