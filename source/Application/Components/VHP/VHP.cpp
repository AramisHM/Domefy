/*
 * File: VHP.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Saturday, 18th May 2019 9:54:36 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 18th May 2019 9:57:13 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#include <Application/Components/VHP/VHP.h>

// constructor and destructor
VHP::VHP(Urho3D::Context* context) : Urho3D::LogicComponent(context) {
    numberOfSlices = 6000;  // Default value for female project
    heightOffset = 5;

    sagitalLevel = 0.0f;
    coronalLevel = 0.0f;
    axialLevel = 0.0f;

    axialLength = 10.0f;
    axialHeight = 5.13f;
}
VHP::~VHP() {}

// Update
void VHP::Update(float timeStep) {}  // TODO: implement it for something maybe?

void VHP::CreateModel() {
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();

    // neutralize the pointers
    for (int h = 0; h < numberOfSlices; ++h) {
        // null initialization, frounding
        slicesMaterials[h] = 0;
        slicesNodes[h] = 0;
    }
    printf("loading..\n");

    // Visible Human Project Axial
    for (int h = 4450; h < numberOfSlices; h = h + 3) {
        Urho3D::Material* mushroomMat = cache->GetResource<Urho3D::Material>(
            "/home/aramis/research/Materials/vhp/axial/" +
            Urho3D::String(6189 - h) + ".xml");

        if (!mushroomMat) {
            slicesMaterials[h] = 0;
            slicesNodes[h] = 0;
            continue;
        }

        std::stringstream ss;
        ss << "VHP-";
        ss << h;
        ss << "\0";

        std::string nodeName = ss.str();
        printf("\n\n added slice %s\n\n", nodeName.c_str());
        Node* someNode;
        someNode = node_;
        Node* floorNode = someNode->CreateChild(nodeName.c_str());
        floorNode->SetPosition(Vector3(0.0f, h * 0.0035f - heightOffset, 0.0f));
        floorNode->SetScale(Vector3(axialLength, 0.0f, axialHeight));
        StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));

        mushroomMat->SetShaderParameter("MatDiffColor",
                                        Color(1.1f, 1.1f, 1.1f, 1.0f));
        floorObject->SetMaterial(mushroomMat);

        // STORE IT FOR LATER REFERENCING
        slicesMaterials[h] = mushroomMat;
        slicesNodes[h] = floorNode;
    }

    // experimental sagital dataset
    for (int h = 1; h < 1000; h = h + 3) {
        Urho3D::Material* m = cache->GetResource<Urho3D::Material>(
            "/home/aramis/research/Materials/vhp/sagital/lowres/" +
            Urho3D::String(h) + ".xml");

        if (!m) {
            continue;
        }

        std::stringstream ss;
        ss << "VHP-LRA-";
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* someNode;
        someNode = node_;
        Node* sliceNode = someNode->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(0.0f, h * 0.0035f - heightOffset, 0.0f));
        sliceNode->SetScale(Vector3(2, 0.0f, 10));
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(cache->GetResource<Model>("Models/Box.mdl"));

        m->SetShaderParameter("MatDiffColor", Color(1.1f, 1.1f, 1.1f, 1.0f));
        sliceMesh->SetMaterial(m);
    }
}

void VHP::SumSagitalCut(float level) {
    sagitalLevel += level;
    for (int h = 4450; h < numberOfSlices; h = h + 3) {
        Material* m = slicesMaterials[h];
        Node* n = slicesNodes[h];

        if (!m || !n) continue;  // no node or material.: ignore

        // m->SetUVTransform(Vector2(sagitalLevel, 0.0f), 0, 1);

        m->SetUVTransform(Vector2(0.5f, 0.0f), 0, Vector2(0.5f, 1.0f));
        n->SetScale(Vector3(axialLength / 2, 0, axialHeight));
    }
}
void VHP::SumCoronal(float level) { coronalLevel += level; }
void VHP::SumAxial(float level) { axialLevel += level; }