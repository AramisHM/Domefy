/*
 * File: ProgramConfig.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 13th June 2019 10:55:14 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 13th June 2019 10:58:31 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#include <Core/ProgramConfig.h>

namespace fpmed {

ProgramConfig* ProgramConfig::_instance = NULL;

// constructor
ProgramConfig::ProgramConfig() {
    nProjections = 0;
    _fullscreen = false;
    _borderless = false;
    _resolution = Vec2<int>(640, 480);
}

ProgramConfig* ProgramConfig::GetInstance() {
    if (_instance == NULL) {
        _instance = new ProgramConfig();
    }
    return (_instance);
}

int ProgramConfig::LoadConfigFile(std::string path) {
    std::ifstream filest(path);
    json myConfig;
    filest >> myConfig;

    _projections;

    // Get projection viewport configurations
    std::string presetFile = myConfig["presets_file"];

    // If any preset file, load it
    if (presetFile.length() > 0) {  // TODO: is there another propper way to
                                    // check if any data has been suplied?
        std::ifstream presetsStream(presetFile);
        json presetsJ;
        presetsStream >> presetsJ;

        if (presetsJ["presets"].size() > 0) {
            nProjections = 0;
            for (const auto& item : presetsJ["presets"]) {
                // new projection reference
                Projection p;

                p.name = item["name"].get<std::string>().c_str();

                Vec3<float> offPos(item["offset_position"]["x"].get<float>(),
                                   item["offset_position"]["y"].get<float>(),
                                   item["offset_position"]["z"].get<float>());
                Vec3<float> offRot(item["offset_rotation"]["x"].get<float>(),
                                   item["offset_rotation"]["y"].get<float>(),
                                   item["offset_rotation"]["z"].get<float>());
                Vec3<float> projPos(item["proj_position"]["x"].get<float>(),
                                    item["proj_position"]["y"].get<float>(),
                                    item["proj_position"]["z"].get<float>());
                Vec3<float> projRot(item["proj_rotation"]["x"].get<float>(),
                                    item["proj_rotation"]["y"].get<float>(),
                                    item["proj_rotation"]["z"].get<float>());

                p._viewport = Vec4<int>(item["viewport_pos"]["x"].get<int>(),
                                        item["viewport_pos"]["y"].get<int>(),
                                        item["resolution"]["x"].get<int>(),
                                        item["resolution"]["y"].get<int>());
                p._domeYaw = item["dome_yaw"].get<float>();

                p._offsetPos = offPos;
                p._offsetRot = offRot;
                p._projPos = projPos;
                p._projRot = projRot;
                p._fov = item["fov"].get<float>();
                p._farClip = item["far_clip"].get<float>();
                p._rttResolution = item["rtt_resolution"].get<int>();

                // load morph mesh for this projection mesh
                for (const auto& line : item["morph_mesh"]) {  // for each line
                    for (const auto& vertex :
                         line) {  // for each column, that happens to be the
                                  // actual vertex itself too
                        MorphVertex mv;
                        mv.index = vertex["index"].get<int>();
                        mv.x = vertex["x"].get<float>();
                        mv.y = vertex["y"].get<float>();
                        p._morphMesh.push_back(
                            mv);  // push vertex into the morphmesh structure
                                  // array
                    }
                }
                p._index = nProjections;
                _projections.push_back(p);  // save for later referencing

                ++nProjections;
            }
        }
    }

    _borderless = myConfig["borderless"].get<bool>();
    _fullscreen = myConfig["fullscreen"].get<bool>();

    _resolution = Vec2<int>(myConfig["master_resolution"]["x"].get<int>(),
                            myConfig["master_resolution"]["y"].get<int>());

    _windowPosition = Vec2<int>(myConfig["window_position"]["x"].get<int>(),
                                myConfig["window_position"]["y"].get<int>());
    _monitor = myConfig["monitor"].get<int>();
#ifdef _WIN32
    _pathToCustomResources =
        myConfig["custom_assets_path"]["win32"].get<std::string>();
#else
    _pathToCustomResources =
        myConfig["custom_assets_path"]["linux"].get<std::string>();
#endif  //_WIN32
    return 1;
}

std::list<Projection> ProgramConfig::GetProjections() { return _projections; }

bool ProgramConfig::IsFullscreen() { return _fullscreen; }
bool ProgramConfig::IsBorderless() { return _borderless; }

Vec2<int> ProgramConfig::GetWindowResolution() { return _resolution; }

unsigned int ProgramConfig::GetLoadedProjectionsCount() { return nProjections; }

unsigned int ProgramConfig::GetMonitor() { return _monitor; }
fpmed::Vec2<int> ProgramConfig::GetWindowPosition() { return _windowPosition; }
std::string ProgramConfig::GetPathToCustomAssetsFolder() {
    return _pathToCustomResources;
}

Urho3D::String ProgramConfig::GetPathToCustomAssetsFolderURHO3D() {
    return Urho3D::String(this->GetPathToCustomAssetsFolder().c_str());
}

}  // namespace fpmed