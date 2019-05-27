/*
 * File: AnatomicViewer.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 25th May 2019 1:43:20 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 25th May 2019 1:43:39 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#include <Application/Components/AnatomicViewer/AnatomicViewer.h>

AnatomicViewer::AnatomicViewer(Context* context)
    : Urho3D::LogicComponent(context) {}

AnatomicViewer::~AnatomicViewer() {}

void AnatomicViewer::Update(float timeStep) {}

void AnatomicViewer::CreateViewer() {
    // used to load resources from the machine filesystem
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();

    this->viewerNode = node_->CreateChild("Viewer");
    this->viewerModelNode = viewerNode->CreateChild("ViewerModel");
    StaticModel* viewerModel = viewerModelNode->CreateComponent<StaticModel>();
    viewerModel->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

    viewerModel->SetMaterial(
        cache->GetResource<Material>("Materials/Stone.xml"));
    viewerModelNode->SetScale(Vector3(
        viewerModel->GetMaterial(0)->GetTexture(TU_DIFFUSE)->GetWidth() / 340,
        0,
        viewerModel->GetMaterial(0)->GetTexture(TU_DIFFUSE)->GetHeight() /
            340));
    // TODO: set the material

    viewerGrab = viewerNode->CreateComponent<GrabbableUI>();
    viewerGrab->SetOrbitableNode(node_);
}

// Passes the data to GrabbableUI to apply movement and momentum
void AnatomicViewer::ApplyMouseMove(Vec2<int> d) {
    viewerGrab->ApplyMouseMove(d);
}