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
// #include <Urho3DAll.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>  // c++ standar vector calss

using namespace Urho3D;
using namespace fpmed;

// NOTE: You must register the component to the engine in order for it to
// properly work use with context_->RegisterFactory<Name of your component>();
// to do so

#define MAX_NUM_SLICES 7000

enum VHPBaseType { sagital = 1,
                   coronal = 2,
                   axial = 3,
                   invalid = 0 };

/// Custom logic component: Used to create the Visible Human
class VHP : public LogicComponent {
    // Must do this to register your class componenet
    URHO3D_OBJECT(VHP, LogicComponent);

   private:
    // level of the anatomic cuts
    float sagitalLevel;
    float coronalLevel;
    float axialLevel;

    // tells if model is laying of standing up
    bool _legt;

    // Slices bases
    Node* coronalBasedDatesed;
    Node* sagitalBasedDatesed;
    Node* axialBasedDatesed;

    // Data dimensions precalculations
    // TODO: Add description to each one of them
    int axialSliceQuantity;
    int sagitalSliceQuantity;
    int coronalSliceQuantity;
    float modelNormalWidth;
    float modelNormalHeight;
    float modelNormalDepth;
    float sliceAxialInterval;
    float sliceSagitalInterval;
    float sliceCoronalInterval;

    // anatomic cuts levels
    float _currentSagitalLeftLevel;
    float _currentSagitalRightLevel;
    float _currentCoronalFrontLevel;
    float _currentCoronalBackLevel;
    float _currentAxialUpperLevel;
    float _currentAxialLowerLevel;

    float _targetSagitalLeftLevel;
    float _targetSagitalRightLevel;
    float _targetCoronalFrontLevel;
    float _targetCoronalBackLevel;
    float _targetAxialUpperLevel;
    float _targetAxialLowerLevel;

    // ongoig cuts indicates if we are in the process of perform a graceful cut,
    // a cut with a transition
    bool _sagitalOngoingCut;
    bool _axialOngoingCut;
    bool _coronalOngoingCut;

    // In case we make a transition, what would be the amount of transformation
    // each time we update the scene. Value should be between 0 and 1
    // (normalized)
    float _transitionFactor;  // TODO: getter setter

    // the entire model transparency and colo
    // TODO: make own functions
    float _modelTransparency;
    Color _modelColor;

    // set of materials and nodes to perform the anatomic cuts
    Material* _axialSlicesMaterials[MAX_NUM_SLICES];
    Node* _axialSlicesNodes[MAX_NUM_SLICES];
    Material* _sagitalSlicesMaterials[MAX_NUM_SLICES];
    Node* _sagialSlicesNodes[MAX_NUM_SLICES];
    Material* _coronalSlicesMaterials[MAX_NUM_SLICES];
    Node* _coronalSlicesNodes[MAX_NUM_SLICES];

    // the node reference that has the camera componenet
    Node* _viewingReferenceNode;

    Node* _rootNode;  // Used to easely position the entire model in relation to
                      // a local origin

    // 1 sagital, 2 coronal, 3 axial and 0 invalid
    VHPBaseType _currentReferenceBase;

    // The variables loaded from the JSON file
    Vec3<float> _dimensions;
    Vec3<int> _amount;
    std::string _axialTexturesPath, _coronalTexturesPath, _sagitalTexturesPath;

    // UpdateAnatomicCuts - Updates the anatomic cuts from all image datases
    void UpdateAnatomicCuts();

   public:
    VHP(Context* context);
    ~VHP();

    // Creates the model from Visible Human Project
    // void CreateModel();
    // Creates the model of Visible Human Project specified in a JSON file.
    void CreateModel(String filePath);

    // Override
    virtual void Update(float timeStep) override;

    // SetSagitalCut - Sets the value from parameter to the sagital cut level
    // graceful does a transition from current level to the new desired value
    void SetSagitalCut(float leftLevel, float rightLevel,
                       bool graceful = false);

    // SetCoronalCut - Sets the value from parameter to the coronal cut level
    void SetCoronalCut(float frontLevel, float backLevel);

    // SetAxialCut - Sets the value from parameter to the axial cut level
    void SetAxialCut(float upperLevel, float lowerLevel);

    // SumCoronalCut - Adds the value from parameter to the coronal cut
    // level
    void SumCoronal(float level);

    // SumAxialCut - Adds the value from parameter to the axial cut level
    void SumAxial(float level);

    // SetSagitalBaseVisible - Enables or disables the rendering of the sagital
    // planes.
    void SetSagitalBaseVisible(bool isVisible);
    // SetCoronalBaseVisible - Enables or disables the rendering of the coronal
    // planes.
    void SetCoronalBaseVisible(bool isVisible);
    // SetAxialBaseVisible - Enables or disables the rendering of the axial
    // planes.
    void SetAxialBaseVisible(bool isVisible);

    // UpdateWhatBaseToShow - Gets relative coordinates to the viwing camera,
    // and define what base to show
    void UpdateWhatBaseToShow();

    // GetCurrentShowingBase - returns 1 if sagital, 2 coronal and 3 axial; 0 if
    // invalid. It indicates what set of images is beeing used to show the model
    // to the viewer
    VHPBaseType GetCurrentShowingBase();

    // SetViewNodeReference - Sets the node that we use as reference to
    // calculate what   base to use
    void SetViewNodeReference(Node* n);

    // SetModelTransparency - Sets the transparency of the model
    void SetModelTransparency(float level);

    // LoadFromFile - Loads a JSON file that defines the dimensions, slice count
    // and textures paths
    void LoadFromFile(std::string filePath);
};

#endif