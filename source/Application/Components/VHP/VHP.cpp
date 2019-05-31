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
    numberOfSlices = 6189;  // Default value for female project
    heightOffset = 5;

    sagitalLevel = 0.0f;
    coronalLevel = 0.0f;
    axialLevel = 0.0f;

    axialLength = 10.0f;
    axialHeight = 5.13f;

    coronalBasedDatesed = sagitalBasedDatesed = axialBasedDatesed = 0;
}
VHP::~VHP() {}

// Update
void VHP::Update(float timeStep) {}  // TODO: implement it for something maybe?

void VHP::CreateModel() {
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Data dimensions precalculations
    int axialSliceQuantity = 5189;
    int sagitalSliceQuantity = 1024;
    int coronalSliceQuantity = 525;

    float modelNormalWidth = 1.0f;
    float modelNormalHeight = 2.5662f;
    float modelNormalDepth = 0.5127f;

    float sliceAxialInterval = (modelNormalHeight / axialSliceQuantity);
    float sliceSagitalInterval = (modelNormalWidth / sagitalSliceQuantity);
    float sliceCoronalInterval = (modelNormalDepth / coronalSliceQuantity);

    // neutralize the pointers
    for (int h = 0; h < numberOfSlices; ++h) {
        // null initialization, frounding
        slicesMaterials[h] = 0;
        slicesNodes[h] = 0;
    }

    //  Lowres Axial
    axialBasedDatesed = node_->CreateChild("lowresAxialSetBase");
    for (int h = 1001; h < 1001 + axialSliceQuantity + 1; ++h) {
        Urho3D::Material* m = cache->GetResource<Urho3D::Material>(
            "/home/aramis/research/Materials/vhp/axial/lowres/" +
            Urho3D::String(h) + ".xml");

        if (!m) continue;

        std::stringstream ss;
        ss << "VHP-LRA-";  // LRA means "low res axial"
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* sliceNode = axialBasedDatesed->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(0.0f, (h * sliceAxialInterval), 0.0f));
        sliceNode->SetScale(Vector3(0.5127f, 0.0f, 1.0f));
        sliceNode->Rotate(Quaternion(180.0f, 0.0f, 0.0f));  // flip sides
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(
            cache->GetResource<Model>("Models/PlaneMirrored.mdl"));

        m->SetShaderParameter("MatDiffColor", Color(4.0f, 4.0f, 4.0f, 0.05f));
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);
    }
    axialBasedDatesed->SetRotation(Quaternion(0.0f, 180.0f, 180.0f));
    axialBasedDatesed->SetScale(5.25f);
    axialBasedDatesed->SetPosition(Vector3(
        0.0f,
        modelNormalHeight * modelNormalHeight +
            (modelNormalWidth * modelNormalHeight) + 0.15f,  // 0.15f is a guess
        0.0f));

    //  Lowres Sagital
    sagitalBasedDatesed = node_->CreateChild("lowresSagitalSetBase");
    for (int h = 0; h < sagitalSliceQuantity + 1; ++h) {
        Urho3D::Material* m = cache->GetResource<Urho3D::Material>(
            "/home/aramis/research/Materials/vhp/sagital/lowres/" +
            Urho3D::String(sagitalSliceQuantity - h) + ".xml");

        if (!m) continue;

        std::stringstream ss;
        ss << "VHP-LRS-";  // LRS means "low res sagital"
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* sliceNode = sagitalBasedDatesed->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(0.0f, (h * sliceSagitalInterval), 0.0f));
        sliceNode->SetScale(Vector3(2.5662f, 0.0f, 0.5127f));
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(
            cache->GetResource<Model>("Models/PlaneMirrored.mdl"));

        m->SetShaderParameter("MatDiffColor", Color(4.0f, 4.0f, 4.0f, 0.05f));
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);
    }
    sagitalBasedDatesed->SetRotation(Quaternion(0.0f, 90.0f, -90.0f));
    sagitalBasedDatesed->SetScale(5.25f);
    sagitalBasedDatesed->SetPosition(
        Vector3(0.0f, 0.0f,
                modelNormalWidth * modelNormalHeight));  // No idea why is this.

    // Lowres Coronal
    coronalBasedDatesed = node_->CreateChild("lowresCoronalSetBase");
    for (int h = 0; h < coronalSliceQuantity + 1; ++h) {
        Urho3D::Material* m = cache->GetResource<Urho3D::Material>(
            "/home/aramis/research/Materials/vhp/coronal/lowres/" +
            Urho3D::String(h) + ".xml");

        if (!m) continue;

        std::stringstream ss;
        ss << "VHP-LRC-";  // LRC means "low res coronal"
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* sliceNode = coronalBasedDatesed->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(0.0f, (h * sliceCoronalInterval), 0.0f));
        sliceNode->SetScale(Vector3(modelNormalHeight, 0.0f, modelNormalWidth));
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(
            cache->GetResource<Model>("Models/PlaneMirrored.mdl"));

        m->SetShaderParameter("MatDiffColor", Color(4.0f, 4.0f, 4.0f, 0.05f));
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);
    }
    coronalBasedDatesed->SetRotation(Quaternion(0.0f, 0.0f, -90.0f));
    coronalBasedDatesed->SetScale(5.25f);
    coronalBasedDatesed->SetPosition(
        Vector3(modelNormalHeight * -modelNormalDepth, 0.0f, 0.0f));

    coronalBasedDatesed->SetEnabledRecursive(false);  // initial state
}

void VHP::SumSagitalCut(float level) {
    sagitalLevel += level;
    for (int h = 4450; h < numberOfSlices; h = h + 3) {
        Material* m = slicesMaterials[h];
        Node* n = slicesNodes[h];

        if (!n || !m) continue;  // no node or material.: ignore

        // m->SetUVTransform(Vector2(sagitalLevel, 0.0f), 0, 1);

        m->SetUVTransform(Vector2(0.5f, 0.0f), 0, Vector2(0.5f, 1.0f));
        n->SetScale(Vector3(axialLength / 2, 0, axialHeight));
    }
}

void VHP::SumCoronal(float level) { coronalLevel += level; }
void VHP::SumAxial(float level) { axialLevel += level; }

void VHP::SetSagitalBaseVisible(bool isVisible) {
    sagitalBasedDatesed->SetEnabledRecursive(isVisible);
}
void VHP::SetCoronalBaseVisible(bool isVisible) {
    coronalBasedDatesed->SetEnabledRecursive(isVisible);
}
void VHP::SetAxialBaseVisible(bool isVisible) {
    axialBasedDatesed->SetEnabledRecursive(isVisible);
}