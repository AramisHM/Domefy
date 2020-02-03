/*
 * File: WebBrowser.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Tuesday, 21st May 2019 4:36:58 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Tuesday, 21st May 2019 4:37:19 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2020 Aramis Hornung Moraes
 */
#ifdef CEF_INTEGRATION
#ifndef WEBBROWSER_H
#define WEBBROWSER_H

#include <Core/CoreParameters.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <iostream>
#include <string>
#include <vector>  // c++ standar vector calss

#include <Urho3D.h>
#include <Urho3DAll.h>

#include <Application/Components/GrabbableUI/GrabbableUI.h>

#include <Urho3D/Scene/LogicComponent.h>

// cef
#include "UBrowserImage.h"

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

/// Custom logic component: Used to load and display WebBrowsers in the 3D
/// scene of the application
class WebBrowser : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(WebBrowser, LogicComponent);

   private:
    // WebBrowserNode is a secondary reference that we place ontop of the camera
    // node. So it can move arround the camera's node
    Node *webBrowserNode_;
    // WebBrowserModel - The model of the scene node
    StaticModel *webBrowserModel_;

    // panoramic 360 warp
    Node *webBrowser360Node_;
    Node *webBrowser360ModelNode_;
    StaticModel *webBrowser360Model_;  // used for 360 panoramic media

    // youtube 360 warp
    Node *webBrowserYoutube360Node_;
    Node *webBrowserYoutube360ModelNode_;
    StaticModel *webBrowserYoutube360Model_;  // used for 360 panoramic media

    // youtube 360 3D warp
    Node *webBrowserYoutube3603DNode_;
    Node *webBrowserYoutube3603DModelNode_;
    StaticModel *webBrowserYoutube3603DModel_;  // used for 360 panoramic media

    // WebBrowserGrabbableUI - We use this to move the WebBrowser arround the
    // camera node, accordingly to a spheric coordinate system
    GrabbableUI *webBrowserGrabbableUI_;

    UBrowserImage *browser_;

    bool UIRender;
    std::string defaultStartUrl;

   public:
    // WebBrowser - The constructor
    WebBrowser(Context *context);
    // ~WebBrowser - The destructor
    ~WebBrowser();
    // CreateWebBrowser - Load data and creates the WebBrowser model into the
    // root node
    void CreateWebBrowser(int resX = 640, int resY = 480);

    void ToggleUIRender();

    // Runs every render loop
    virtual void Update(float timeStep) override;

    // Passes the data to GrabbableUI to apply movement and momentum
    void ApplyMouseMoveLegacy(Vec2<int> d);
    // Passes the data to GrabbableUI to apply movement and momentum with
    // Urho3D's IntVector2
    void ApplyMouseMove(Urho3D::IntVector2 d);
    void SetZoom(float zoom);
    // Similar to SetZoom, but the float value is added to the current zoom
    // value, instead of set.
    void AddZoom(float zoom);

    void SetSphericView(bool isSpheric);
    void SetCubeView(bool isCubic);
    void SetCube3DView(bool isCubic);

    UBrowserImage *GetBrowserImage();

    void LoadURL(std::string url);
    void DefaultURL();

    GrabbableUI *GetGrabbableUI();
};

#endif
#endif