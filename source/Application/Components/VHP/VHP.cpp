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
}
VHP::~VHP() {}

// Update
void VHP::Update(float timeStep) {}  // TODO: implement it for something maybe?

void VHP::CreateModel() {
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();

    // neutralize the pointers
    for (int h = 0; h < numberOfSlices; ++h) {
        slicesMaterials[h] = 0;  // null initialization
    }
    printf("carregando..\n");
    // Visible Human Project - VHP
    for (int h = 4450; h < numberOfSlices; h = h + 3) {
        Urho3D::Material* mushroomMat = cache->GetResource<Urho3D::Material>(
            "/home/aramis/research/Materials/vhp/axial/" +
            Urho3D::String(6189 - h) + ".xml");

        if (!mushroomMat) {
            slicesMaterials[h] = 0;
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
        floorNode->SetScale(Vector3(10.0f, 0.0f, 5.13f));
        StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));

        mushroomMat->SetShaderParameter("MatDiffColor",
                                        Color(1.1f, 1.1f, 1.1f, 1.0f));
        floorObject->SetMaterial(mushroomMat);
        slicesMaterials[h] = mushroomMat;  // STORE IT FOR LATER REFERENCING
    }
}

void VHP::SumSagitalCut(float level) {
    sagitalLevel += level;
    for (int h = 4450; h < numberOfSlices; h = h + 3) {
        Material* m = slicesMaterials[h];

        if (!m) continue;  // ignore
        m->SetUVTransform(Vector2(sagitalLevel, 0.0f), 0, 1);
    }
}
void VHP::SumCoronal(float level) { coronalLevel += level; }
void VHP::SumAxial(float level) { axialLevel += level; }