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
#include <Core/Projector.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <Math/vector4.h>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

#include <fstream>
#include <streambuf>

using json = nlohmann::json;

// Pojection structure that holds the projection viewpoerts
typedef struct Pojection {
    std::string name;
    fpmed::Vec3<float> _offsetPos;
    fpmed::Vec3<float> _offsetRot;
    fpmed::Vec3<float> _projPos;
    fpmed::Vec3<float> _projRot;
    fpmed::Vec4<int> _viewport;
    float fov;
} Projection;

// Singleton class that holds all configurations for our software.

namespace fpmed {

class ProgramConfig {
   public:
    // This is how clients can access the single instance
    static ProgramConfig *GetInstance();

    int LoadConfigFile(std::string path);
    // GetProjections - Returns the std::list of viewport projections.
    std::list<Projection> GetProjections();

    bool IsFullscreen();

    // IsBorderless - Determins if the window instance should render the frame
    // arround it
    bool IsBorderless();

    // WindowResolution - What is the resolution of the application window.
    Vec2<int> GetWindowResolution();

    unsigned int GetLoadedProjectionsCount();

   protected:
    std::list<Projection> _projections;
    unsigned int nProjections;  // number of projections loaded
    bool _fullscreen;
    bool _borderless;
    Vec2<int> _resolution;

   private:
    static ProgramConfig *_instance;
    ProgramConfig();
    ProgramConfig(const ProgramConfig &);
    ProgramConfig &operator=(const ProgramConfig &);
};

}  // namespace fpmed

#endif