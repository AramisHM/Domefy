/*
 * File: ProgramConfig.h
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 13th June 2019 10:58:13 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 13th June 2019 10:58:21 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#ifndef PROGRAMCONFIG_H
#define PROGRAMCONFIG_H
#include <Core/CoreParameters.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <Math/vector4.h>
#include <Urho3DAll.h>  // urho3d types

#include <fstream>
#include <list>
#include <nlohmann/json.hpp>
#include <streambuf>
#include <string>

using json = nlohmann::json;

typedef struct MorphVertex {
    int index;
    float x;
    float y;
} MorphVertex;

// Pojection structure that holds the projection viewpoerts
typedef struct Pojection {
    std::string name;
    fpmed::Vec3<float> _offsetPos;
    fpmed::Vec3<float> _offsetRot;
    fpmed::Vec3<float> _projPos;
    fpmed::Vec3<float> _projRot;
    fpmed::Vec4<int> _viewport;
    float _domeYaw;
    float _fov;
    float _farClip;
    unsigned int _rttResolution;
    unsigned int _index;  // internal numeration for each projection loaded

    // the mesh that holds the mesh transformation for point-to-point
    // calibration
    std::vector<MorphVertex> _morphMesh;

    // customCorrectionMeshPath - Used to load custom mesh, with calibration
    // done externally, e.g. Blender.
    std::string _customCorrectionMeshPath;
    bool _hasCustomCorrectionMesh;
} Projection;

// Singleton class that holds all configurations for our software.

namespace fpmed {

class ProgramConfig {
   public:
    // This is how clients can access the single instance
    static ProgramConfig *GetInstance();

    int LoadConfigFile(std::string path, std::string presFile);
    // GetProjections - Returns the std::list of viewport projections.
    std::list<Projection> GetProjections();

    bool IsFullscreen();

    // IsBorderless - Determins if the window instance should render the frame
    // arround it
    bool IsBorderless();

    // WindowResolution - What is the resolution of the application window.
    Vec2<int> GetWindowResolution();

    unsigned int GetLoadedProjectionsCount();
    unsigned int GetMonitor();
    fpmed::Vec2<int> GetWindowPosition();
    // GetPathToCustomAssetsFolder - Includes path that leads to e.g the VHP
    // images
    std::string GetPathToCustomAssetsFolder();
    Urho3D::String GetPathToCustomAssetsFolderURHO3D();

    // These are required by AngelScript
    void AddRef() { /* do nothing */
    }
    void ReleaseRef() { /* do nothing */
    }

   protected:
    std::list<Projection> _projections;
    unsigned int nProjections;  // number of projections loaded
    bool _fullscreen;
    bool _borderless;
    Vec2<int> _resolution;
    Vec2<int> _windowPosition;
    unsigned int _monitor;

    // misc
    std::string
        _pathToCustomResources;  // path that leads to e.g the VHP images

   private:
    static ProgramConfig *_instance;
    ProgramConfig();
    ProgramConfig(const ProgramConfig &);
    ProgramConfig &operator=(const ProgramConfig &);
};

}  // namespace fpmed

#endif