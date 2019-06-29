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
#include <Core/ProgramConfig.h>
#include <Core/auxiliarAppFuncs.h>  // very low level functions

// constructor and destructor
VHP::VHP(Urho3D::Context* context) : Urho3D::LogicComponent(context) {
    sagitalLevel = 0.0f;
    coronalLevel = 0.0f;
    axialLevel = 0.0f;

    coronalBasedDatesed = sagitalBasedDatesed = axialBasedDatesed = 0;

    _currentReferenceBase = invalid;
    _modelColor = Color(7.0f, 7.0f, 7.0f, 3.0f);

    _currentSagitalLeftLevel = 0.0f;
    _currentSagitalRightLevel = 0.0f;
    _currentCoronalFrontLevel = 0.0f;
    _currentCoronalBackLevel = 0.0f;
    _currentAxialUpperLevel = 0.0f;
    _currentAxialLowerLevel = 0.0f;
    _targetSagitalLeftLevel = 0.0f;
    _targetSagitalRightLevel = 0.0f;
    _targetCoronalFrontLevel = 0.0f;
    _targetCoronalBackLevel = 0.0f;
    _targetAxialUpperLevel = 0.0f;
    _targetAxialLowerLevel = 0.0f;

    _sagitalOngoingCut = false;
    _axialOngoingCut = false;
    _coronalOngoingCut = false;

    _transitionFactor = 0.03f;

    _legt = true;
}
VHP::~VHP() {}

// Update
void VHP::Update(float timeStep) {
    UpdateWhatBaseToShow();
    UpdateAnatomicCuts();
}

void VHP::CreateModel(std::string filePath) {
    this->LoadFromFile(filePath);
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();
    // fpmed::ProgramConfig* config = fpmed::ProgramConfig::GetInstance();

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

    _rootNode = node_->CreateChild("vhpRoot");

    //  Lowres Axial ------------------------------------------------
    axialBasedDatesed = _rootNode->CreateChild("axialSetBase");
    for (int h = 0; h < axialSliceQuantity; h += 900) {
        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(0, cache->GetResource<Technique>(
                               "Techniques/DiffAlphaTranslucent.xml"));
        Urho3D::Texture2D* t = cache->GetResource<Urho3D::Texture2D>(
            Urho3D::String(_axialTexturesPath.c_str()) + "/" +
            Urho3D::String(1001 + h) + ".png");
        if (!t) continue;
        t->SetFilterMode(FILTER_NEAREST_ANISOTROPIC);
        t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
        t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
        t->SetNumLevels(1);
        m->SetTexture(TU_DIFFUSE, t);

        std::stringstream ss;
        ss << "VHP-LRA-";  // LRA means "low res axial"
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* sliceNode = axialBasedDatesed->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(
            0.0f, ((axialSliceQuantity - h) * sliceAxialInterval), 0.0f));
        sliceNode->SetScale(Vector3(modelNormalWidth, 0.0f, modelNormalDepth));
        // sliceNode->Rotate(Quaternion(180.0f, 0.0f, 0.0f));  // flip sides
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(
            cache->GetResource<Model>("Models/DoubleSidedMirroredPlane.mdl"));

        m->SetShaderParameter("MatDiffColor", _modelColor);
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);

        // save pointers
        _axialSlicesNodes[h] = sliceNode;
        _axialSlicesMaterials[h] = m;
    }
    // axialBasedDatesed->SetRotation(Quaternion(0.0f, 180.0f, 180.0f));
    // axialBasedDatesed->SetPosition(Vector3(modelNormalDepth / 2, 0.0f,
    // 0.0f));

    //  Lowres Sagital ------------------------------------------------
    sagitalBasedDatesed = _rootNode->CreateChild("sagitalSetBase");
    for (int h = 0; h < sagitalSliceQuantity; h += 100) {
        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(0, cache->GetResource<Technique>(
                               "Techniques/DiffAlphaTranslucent.xml"));
        Urho3D::Texture2D* t = cache->GetResource<Urho3D::Texture2D>(
            Urho3D::String(_sagitalTexturesPath.c_str()) + "/" +
            Urho3D::String(sagitalSliceQuantity - h) + ".png");
        if (!t) continue;
        t->SetFilterMode(FILTER_NEAREST_ANISOTROPIC);
        t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
        t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
        t->SetNumLevels(1);
        m->SetTexture(TU_DIFFUSE, t);

        std::stringstream ss;
        ss << "VHP-LRS-";  // LRS means "low res sagital"
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* sliceNode = sagitalBasedDatesed->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(0.0f, (h * sliceSagitalInterval), 0.0f));
        sliceNode->SetScale(Vector3(modelNormalDepth, 0.0f, modelNormalHeight));
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(
            cache->GetResource<Model>("Models/DoubleSidedMirroredPlane.mdl"));

        m->SetShaderParameter("MatDiffColor", _modelColor);
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);

        // save pointers
        _sagialSlicesNodes[h] = sliceNode;
        _sagitalSlicesMaterials[h] = m;
    }
    sagitalBasedDatesed->SetRotation(Quaternion(-90.0f, 90.0f, 0.0f));
    sagitalBasedDatesed->SetPosition(
        Vector3(modelNormalWidth, 0.0f, modelNormalDepth));

    // Lowres Coronal ------------------------------------------------
    coronalBasedDatesed = _rootNode->CreateChild("coronalSetBase");
    for (int h = 0; h < coronalSliceQuantity; h += 2) {
        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(0, cache->GetResource<Technique>(
                               "Techniques/DiffAlphaTranslucent.xml"));
        Urho3D::Texture2D* t = cache->GetResource<Urho3D::Texture2D>(
            Urho3D::String(_coronalTexturesPath.c_str()) + "/" +
            Urho3D::String(h) + ".png");
        if (!t) continue;
        t->SetFilterMode(FILTER_NEAREST_ANISOTROPIC);
        t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
        t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
        t->SetNumLevels(1);
        m->SetTexture(TU_DIFFUSE, t);

        std::stringstream ss;
        ss << "VHP-LRC-";  // LRC means "low res coronal"
        ss << h;
        ss << "\0";
        std::string nodeName = ss.str();

        Node* sliceNode = coronalBasedDatesed->CreateChild(nodeName.c_str());
        sliceNode->SetPosition(Vector3(0.0f, (h * sliceCoronalInterval), 0.0f));
        sliceNode->SetScale(Vector3(modelNormalWidth, 0.0f, modelNormalHeight));
        StaticModel* sliceMesh = sliceNode->CreateComponent<StaticModel>();
        sliceMesh->SetModel(
            cache->GetResource<Model>("Models/DoubleSidedMirroredPlane.mdl"));

        m->SetShaderParameter("MatDiffColor", _modelColor);
        // m->SetTechnique(0, cache->GetResource<Technique>(
        //                        "Techniques/DiffAlphaTranslucent.xml"));
        sliceMesh->SetMaterial(m);

        // save pointers
        _coronalSlicesNodes[h] = sliceNode;
        _coronalSlicesMaterials[h] = m;
    }
    coronalBasedDatesed->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
    coronalBasedDatesed->SetPosition(Vector3(0.0f, 0.0f, modelNormalDepth));

    _rootNode->SetPosition(Vector3(
        (float)(-modelNormalWidth / 2), 0.0f,
        (float)(-modelNormalDepth / 2)));  // this way we shift the center to
                                           // the middle of the model
}

void VHP::SetSagitalCut(float leftLevel, float rightLevel, bool graceful) {
    _targetSagitalLeftLevel = leftLevel;
    _targetSagitalRightLevel = rightLevel;
    _sagitalOngoingCut = graceful;  // prototype only in sagital level
}

void VHP::SetCoronalCut(float frontLevel, float backLevel) {
    // TODO: should be target not current. since mechanism is not implemented to
    // make the transition we are setting it directly for now
    _currentCoronalFrontLevel = frontLevel;
    _currentCoronalBackLevel = backLevel;
}

void VHP::SetAxialCut(float upperLevel, float lowerLevel) {
    // TODO: should be target not current. since mechanism is not implemented to
    // make the transition we are setting it directly for now
    _currentAxialLowerLevel = lowerLevel;
    _currentAxialUpperLevel = upperLevel;
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

        if (_legt == false) {
            if ((camLat > 45 && camLat < 91)) {
                SetAxialBaseVisible(true);
                SetSagitalBaseVisible(false);
                SetCoronalBaseVisible(false);
                _currentReferenceBase = axial;
            } else {
                SetAxialBaseVisible(false);
                if ((yAngle > 45 && yAngle < 135) ||
                    (yAngle > 225 && yAngle < 315)) {
                    SetSagitalBaseVisible(false);
                    SetCoronalBaseVisible(true);
                    _currentReferenceBase = coronal;
                } else {
                    SetSagitalBaseVisible(true);
                    SetCoronalBaseVisible(false);
                    _currentReferenceBase = sagital;
                }
            }
        } else {
            if ((camLat > 15 && camLat < 91)) {
                SetAxialBaseVisible(false);
                SetSagitalBaseVisible(false);
                SetCoronalBaseVisible(true);
                _currentReferenceBase = coronal;
            } else {
                SetCoronalBaseVisible(false);
                if ((yAngle > 45 && yAngle < 135) ||
                    (yAngle > 225 && yAngle < 315)) {
                    SetSagitalBaseVisible(false);
                    SetAxialBaseVisible(true);
                    _currentReferenceBase = axial;
                } else {
                    SetSagitalBaseVisible(true);
                    SetAxialBaseVisible(false);
                    _currentReferenceBase = sagital;
                }
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

void VHP::UpdateAnatomicCuts() {
    // Y-axis (axial) base
    if (_currentReferenceBase == axial) {
        // precalculate occluding planes
        int uc, lc;  // upper cut and lower cut
        uc = axialSliceQuantity * _currentAxialUpperLevel;
        lc = axialSliceQuantity * _currentAxialLowerLevel;

        for (int h = 0; h < axialSliceQuantity; ++h) {
            Material* m = _axialSlicesMaterials[h];
            Node* n = _axialSlicesNodes[h];
            if (!n || !m) continue;  // no node or material.: ignore

            // Scale
            n->SetScale(Vector3(
                modelNormalWidth * (1.0f - (_currentSagitalLeftLevel +
                                            _currentSagitalRightLevel)),  // x
                0,                                                        // y
                modelNormalDepth * (1.0f - (_currentCoronalBackLevel +
                                            _currentCoronalFrontLevel))));  // z

            // Position
            n->SetPosition(
                Vector3(modelNormalWidth * _currentSagitalLeftLevel,     // x
                        (axialSliceQuantity - h) * sliceAxialInterval,   // y
                        modelNormalDepth * _currentCoronalFrontLevel));  // z
            // UV
            m->SetUVTransform(
                Vector2(_currentSagitalLeftLevel,
                        _currentCoronalBackLevel),  // offset x,y
                0,                                  // rotation
                Vector2(1.0f - (_currentSagitalLeftLevel +
                                _currentSagitalRightLevel),
                        1.0f - (_currentCoronalBackLevel +
                                _currentCoronalFrontLevel)));  // repeat x, y

            // Occlusion
            if (h < uc || h > (axialSliceQuantity - lc)) {
                n->SetEnabled(false);
            } else {
                n->SetEnabled(true);
            }
        }
    }

    // X-axis (sagital) base
    // transition
    if (_sagitalOngoingCut == true) {
        // left
        {
            // get transition direction
            int direction =
                (_currentSagitalLeftLevel - _targetSagitalLeftLevel > 0 ? -1
                                                                        : 1);

            _currentSagitalLeftLevel += (_transitionFactor * direction);

            _currentSagitalLeftLevel =
                (_currentSagitalLeftLevel > _targetSagitalLeftLevel
                     ? _targetSagitalLeftLevel
                     : _currentSagitalLeftLevel);
        }
        // right
        {
            // get transition direction
            int direction =
                (_currentSagitalRightLevel - _targetSagitalRightLevel > 0 ? -1
                                                                          : 1);

            _currentSagitalRightLevel += (_transitionFactor * direction);

            _currentSagitalRightLevel =
                (_currentSagitalRightLevel > _targetSagitalRightLevel
                     ? _targetSagitalRightLevel
                     : _currentSagitalRightLevel);
        }

        if (_currentSagitalLeftLevel == _targetSagitalLeftLevel &&
            _currentSagitalRightLevel == _targetSagitalRightLevel) {
            _sagitalOngoingCut = false;
        }
    }
    if (_currentReferenceBase == sagital) {
        // precalculate occluding planes
        int lc, rc;  // left cut and right cut
        lc = sagitalSliceQuantity * _currentSagitalLeftLevel;
        rc = sagitalSliceQuantity * _currentSagitalRightLevel;

        for (int h = 0; h < sagitalSliceQuantity; ++h) {
            Material* m = _sagitalSlicesMaterials[h];
            Node* n = _sagialSlicesNodes[h];
            if (!n || !m) continue;  // no node or material.: ignore

            // Scale
            n->SetScale(Vector3(
                modelNormalDepth * (1.0f - (_currentCoronalBackLevel +
                                            _currentCoronalFrontLevel)),  // x
                0,                                                        // y
                modelNormalHeight * (1.0f - (_currentAxialUpperLevel +
                                             _currentAxialLowerLevel))));  // z

            // Position
            n->SetPosition(
                Vector3(modelNormalDepth * _currentCoronalBackLevel,    // x
                        (h * sliceSagitalInterval),                     // y
                        modelNormalHeight * _currentAxialLowerLevel));  // z
            // UV
            m->SetUVTransform(
                Vector2(_currentCoronalBackLevel,
                        _currentAxialUpperLevel),  // offset x,y
                0,                                 // rotation
                Vector2(1.0f - (_currentCoronalFrontLevel +
                                _currentCoronalBackLevel),
                        1.0f - (_currentAxialUpperLevel +
                                _currentAxialLowerLevel)));  // repeat x, y

            // Occlusion
            if (h < rc || h > (sagitalSliceQuantity - lc)) {
                n->SetEnabled(false);
            } else {
                n->SetEnabled(true);
            }
        }
    }

    // Z-axis (coronal) base
    if (_currentReferenceBase == coronal) {
        // precalculate occluding planes
        int fc, bc;  // front and back cut qauntity
        fc = coronalSliceQuantity * _currentCoronalFrontLevel;
        bc = coronalSliceQuantity * _currentCoronalBackLevel;

        for (int h = 0; h < coronalSliceQuantity; ++h) {
            Material* m = _coronalSlicesMaterials[h];
            Node* n = _coronalSlicesNodes[h];
            if (!n || !m) continue;  // no node or material.: ignore

            // Scale
            n->SetScale(Vector3(
                modelNormalWidth * (1.0f - (_currentSagitalLeftLevel +
                                            _currentSagitalRightLevel)),  // x
                0,                                                        // y
                modelNormalHeight * (1.0f - (_currentAxialUpperLevel +
                                             _currentAxialLowerLevel))));  // z

            // Position
            n->SetPosition(
                Vector3(modelNormalWidth * _currentSagitalLeftLevel,    // x
                        (h * sliceCoronalInterval),                     // y
                        modelNormalHeight * _currentAxialLowerLevel));  // z
            // UV
            m->SetUVTransform(
                Vector2((_currentSagitalLeftLevel), _currentAxialUpperLevel), 0,
                Vector2(1.0f - (_currentSagitalLeftLevel +
                                _currentSagitalRightLevel),
                        1.0f - (_currentAxialUpperLevel +
                                _currentAxialLowerLevel)));

            // Occlusion
            // this is confusing becaue the image
            // base starts from back to front. It is
            // indeed tricky, might need a flag to invert.
            if (h < bc || h > (coronalSliceQuantity - fc)) {
                n->SetEnabled(false);
            } else {
                n->SetEnabled(true);
            }
        }
    }
}

void VHP::LoadFromFile(std::string filePath) {
    // TODO: TO BE CODED

    std::string rootPath;
    rootPath = filePath.substr(0, filePath.find_last_of("\\/"));

    std::ifstream filest(filePath);
    json myConfig;
    filest >> myConfig;

    _dimensions = Vec3<float>(myConfig["dimensions"]["x"].get<float>(),
                              myConfig["dimensions"]["y"].get<float>(),
                              myConfig["dimensions"]["z"].get<float>());

    _amount = Vec3<int>(myConfig["amount"]["x"].get<int>(),
                        myConfig["amount"]["y"].get<int>(),
                        myConfig["amount"]["z"].get<int>());

    _axialTexturesPath =
        rootPath + "/" + myConfig["textures"]["axial"].get<std::string>();
    _coronalTexturesPath =
        rootPath + "/" + myConfig["textures"]["coronal"].get<std::string>();
    _sagitalTexturesPath =
        rootPath + "/" + myConfig["textures"]["sagital"].get<std::string>();

    // Data dimensions precalculations
    axialSliceQuantity = _amount.getY();
    sagitalSliceQuantity = _amount.getX();
    coronalSliceQuantity = _amount.getZ();

    modelNormalWidth = _dimensions.getX();
    modelNormalHeight = _dimensions.getY();
    modelNormalDepth = _dimensions.getZ();

    sliceAxialInterval = (modelNormalHeight / axialSliceQuantity);
    sliceSagitalInterval = (modelNormalWidth / sagitalSliceQuantity);
    sliceCoronalInterval = (modelNormalDepth / coronalSliceQuantity);
}