/*
 * File: Slide.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Tuesday, 21st May 2019 4:36:58 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Tuesday, 21st May 2019 4:37:19 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef SLIDE_H
#define SLIDE_H

#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Core/CoreParameters.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <Urho3D.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3DAll.h>

#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss
using namespace Urho3D;
using namespace fpmed;

// The maximum number of slides that we will expect in a single presentation.
#define MAX_SLIDE_COUNT 1024

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

/// Custom logic component: Used to load and display slides in the 3D
/// scene of the application
class Slide : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(Slide, LogicComponent);

   private:
    // this holds the individual slides materials
    Texture2D *slideTextureArray[MAX_SLIDE_COUNT];

    // the texts for interest points (they float arroud the scene, its a
    // prototype)
    // TODO: move it to it own componenet
    Node *interestPointTexts_[1024];

    // auxiliar slide, used, for example, given a fulldome theaterm, the
    // presenter, that might be facing back the main slide, can see what slide
    // he/she is on with the mirror slide
    bool useMirrorSlide;

    // slideNode is a secondary reference that we place ontop of the camera
    // node. So it can move arround the camera's node
    Node *slideNode;
    Node *mirrorSlideNode;  // secondary slide node

    // slideGrabbableUI - We use this to move the slide arround the camera node,
    // accordingly to a spheric coordinate system
    GrabbableUI *slideGrabbableUI;
    GrabbableUI *mirrorSlideGrabbableUI;

    // slideModel - the master-slide model reference, used for swap slides on
    // the fly. Saves use one step, had we try to get it by the node children.
    StaticModel *slideModel;
    StaticModel *mirrorSlideModel;

    // Simple indicator of wich slide we currently are.
    int currentSlideIndex;

    int nLoadedSlides;

   public:
    // Slide - The constructor
    Slide(Context *context);
    // ~Slide - The destructor
    ~Slide();
    // CreateSlide - Load data and creates the slide model into the root node
    void CreateSlide(Urho3D::String filePath);

    // Runs every render loop
    virtual void Update(float timeStep) override;

    // Passes the data to GrabbableUI to apply movement and momentum
    void ApplyMouseMoveLegacy(Vec2<int> d);
    // Passes the data to GrabbableUI to apply movement and momentum with
    // Urho3D's IntVector2
    void ApplyMouseMove(Urho3D::IntVector2 d);
    void SetCoordinates(Urho3D::IntVector2 d);
    void SetZoom(float zoom);

    void MirrorApplyMouseMove(Urho3D::IntVector2 d);
    void MirrorSetCoordinates(Urho3D::IntVector2 d);
    void MirrorSetZoom(float zoom);

    // Go to next slide
    void NextSlide();

    // Go to previous slide
    void PreviousSlide();

    // enable or disable the mirror slider
    void ToggleMirrorSlide();

    int LoadSlideFromJSON(std::string path);
};

#endif
