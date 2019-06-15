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
    int presetsSize = myConfig["presets"].size();
    printf("Presets count: %d", presetsSize);
    for (const auto& item : myConfig["presets"]) {
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
        p._offsetPos = offPos;
        p._offsetRot = offRot;
        p._projPos = projPos;
        p._projRot = projRot;
        p.fov = item["fov"].get<float>();

        _projections.push_back(p);  // save for later referencing
        ++nProjections;
    }

    _borderless = myConfig["borderless"].get<bool>();
    _fullscreen = myConfig["fullscreen"].get<bool>();

    _resolution = Vec2<int>(myConfig["master-resolution"]["x"].get<int>(),
                            myConfig["master-resolution"]["y"].get<int>());
}

std::list<Projection> ProgramConfig::GetProjections() { return _projections; }

bool ProgramConfig::IsFullscreen() { return _fullscreen; }
bool ProgramConfig::IsBorderless() { return _borderless; }
Vec2<int> ProgramConfig::GetWindowResolution() { return _resolution; }

}  // namespace fpmed