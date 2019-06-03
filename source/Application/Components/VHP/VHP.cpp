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

// we get the viewing's camera grabui compoenent at some point
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/VHP/VHP.h>
#include <Core/auxiliarAppFuncs.h>  // very low level functions

// constructor and destructor
VHP::VHP(Urho3D::Context* context) : Urho3D::LogicComponent(context) {
    numberOfSlices = 6189;  // Default value for female project
    heightOffset = 5;

    sagitalLevel = 0.0f;
    coronalLevel = 0.0f;
    axialLevel = 0.0f;

    coronalBasedDatesed = sagitalBasedDatesed = axialBasedDatesed = 0;

    // Data dimensions precalculations for the female set
    // TODO: make a function to initialize these
    axialSliceQuantity = 5189;
    sagitalSliceQuantity = 1024;
    coronalSliceQuantity = 525;

    modelNormalWidth = 1.0f;
    modelNormalHeight = 2.5662f;
    modelNormalDepth = 0.5127f;

    sliceAxialInterval = (modelNormalHeight / axialSliceQuantity);
    sliceSagitalInterval = (modelNormalWidth / sagitalSliceQuantity);
    sliceCoronalInterval = (modelNormalDepth / coronalSliceQuantity);

    _currentReferenceBase = VHPBaseType::invalid;  // 4 means invalid

    _modelColor = Color(7.0f, 7.0f, 7.0f, 3.0f);
}
VHP::~VHP() {}

// Update
void VHP::Update(float timeStep) { UpdateWhatBaseToShow(); }

void VHP::CreateModel() {
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();

    // neutralize the pointers
    for (int h = 0; h < MAX_NUM_SLICES; ++h) {
        // null initialization, frounding
        _axialSlicesMaterials[h] = 0;
        _axialSlicesNodes[h] = 0;
        _sagitalSlicesMaterials[h] = 0;
        _sagialSlicesNodes[h] = 0;
        _coronalSlicesMaterials[h] = 0;
        _coronalSlicesNodes[h] = 0;
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

        m->SetShaderParameter("MatDiffColor", _modelColor);
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);

        // save pointers
        _axialSlicesNodes[h] = sliceNode;
        _axialSlicesMaterials[h] = m;
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
    for (int h = 1; h < sagitalSliceQuantity + 1; ++h) {
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

        m->SetShaderParameter("MatDiffColor", _modelColor);
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);

        // save pointers
        _sagialSlicesNodes[h] = sliceNode;
        _sagitalSlicesMaterials[h] = m;
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

        m->SetShaderParameter("MatDiffColor", _modelColor);
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);

        // save pointers
        _coronalSlicesNodes[h] = sliceNode;
        _coronalSlicesMaterials[h] = m;
    }
    coronalBasedDatesed->SetRotation(Quaternion(0.0f, 0.0f, -90.0f));
    coronalBasedDatesed->SetScale(5.25f);
    coronalBasedDatesed->SetPosition(
        Vector3(modelNormalHeight * -modelNormalDepth, 0.0f, 0.0f));

    coronalBasedDatesed->SetEnabledRecursive(false);  // initial state
}

// TODO: considerate what base is beeing shown
// at the moment only works for axial image-set
void VHP::SumSagitalCut(float level) {
    sagitalLevel += level;
    for (int h = 0; h < axialSliceQuantity; ++h) {
        Material* m = _axialSlicesMaterials[h];
        Node* n = _axialSlicesNodes[h];

        if (!n || !m) continue;  // no node or material.: ignore

        // m->SetUVTransform(Vector2(sagitalLevel, 0.0f), 0, 1);

        m->SetUVTransform(Vector2(0.5f, 0.0f), 0, Vector2(0.5f, 1.0f));
        n->SetScale(Vector3(modelNormalWidth / 2, 0, modelNormalDepth));
        n->SetPosition(
            Vector3(0.0f, (h * sliceAxialInterval),
                    -modelNormalDepth /
                        2));  // these values are very confusing... TODO: make
                              // model in blender that results in calculations
                              // that makes more sense
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

void VHP::UpdateWhatBaseToShow() {
    if (_viewingReferenceNode) {
        Vector3 aPos = node_->GetPosition();
        Vector3 bPos = _viewingReferenceNode->GetPosition();

        double yAngle = triangulateTarget(aPos.x_, aPos.y_, aPos.z_, bPos.x_,
                                          bPos.y_, bPos.z_);
        GrabbableUI* camGrab =
            _viewingReferenceNode->GetComponent<GrabbableUI>();
        Vec2<float> camCoords = camGrab->GetCoordinates();
        float camLat = abs(camCoords.getY());

        if ((camLat > 45 && camLat < 91)) {
            SetAxialBaseVisible(true);
            SetSagitalBaseVisible(false);
            SetCoronalBaseVisible(false);
            _currentReferenceBase = VHPBaseType::axial;
        } else {
            SetAxialBaseVisible(false);
            if ((yAngle > 45 && yAngle < 135) ||
                (yAngle > 225 && yAngle < 315)) {
                SetSagitalBaseVisible(true);
                SetCoronalBaseVisible(false);
                _currentReferenceBase = VHPBaseType::sagital;
            } else {
                SetSagitalBaseVisible(false);
                SetCoronalBaseVisible(true);
                _currentReferenceBase = VHPBaseType::coronal;
            }
        }
    }
}

VHPBaseType VHP::GetCurrentShowingBase() { return _currentReferenceBase; }

void VHP::SetViewNodeReference(Node* n) { _viewingReferenceNode = n; }

void VHP::SetModelTransparency(float level) {
    _modelTransparency = level;

    for (int h = 0; h < axialSliceQuantity; ++h) {
        Material* m = _axialSlicesMaterials[h];

        if (!m) continue;  // no node or material.: ignore

        Variant va = Variant(Vector4(_modelColor.r_, _modelColor.g_,
                                     _modelColor.b_, _modelTransparency));
        m->SetShaderParameter("MatDiffColor", va);
    }
    for (int h = 0; h < sagitalSliceQuantity; ++h) {
        Material* m = _sagitalSlicesMaterials[h];

        if (!m) continue;  // no node or material.: ignore

        Variant va = Variant(Vector4(_modelColor.r_, _modelColor.g_,
                                     _modelColor.b_, _modelTransparency));
        m->SetShaderParameter("MatDiffColor", va);
    }
    for (int h = 0; h < coronalSliceQuantity; ++h) {
        Material* m = _coronalSlicesMaterials[h];

        if (!m) continue;  // no node or material.: ignore

        Variant va = Variant(Vector4(_modelColor.r_, _modelColor.g_,
                                     _modelColor.b_, _modelTransparency));
        m->SetShaderParameter("MatDiffColor", va);
    }
}
