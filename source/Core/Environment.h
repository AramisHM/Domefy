/*	This file is part of the FpMED (Anydome) Project Copyright (c) 2014-2016
   Aramis Hornung Moraes See FpmedCore.h for license and conditions for using
   this code
*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <Core/CoreParameters.h>
#include <Core/Projector.h>
#include <Math/vector2.h>
#include <Math/vector3.h>
#include <Math/vector4.h>
#include <vector>  // c++ vector calss for the projectors array, nothing to do with the vec2,3,4 that are graphic vectors

namespace fpmed {
class Environment {
   private:
    char infoName[MediumDataLen], manufacturerName[MediumDataLen],
        authorName[MediumDataLen];
    fpmed::Vec2<float> location;  // location of the digital theater
    // General atributes for environment
    int projection_type,
        projection_configuration;  // Type refers to either fulldome or planar.
                                   // Configuration refers to the way the system
                                   // is displaying, i.e. 3 monitor in line,
                                   // duet fulldome with stereoscopy etc...
    fpmed::Vec2<int>
        resolution;  // You may note by now that we only work with multiple
                     // equipaments but with the same resolution.
    int fullScreen;
    // Only for plannar projections
    int screen_coluns, screen_rows;
    std::vector<fpmed::Projector> environmentProjectors;  // array de projetores

    // Fulldome properties
    fpmed::Vec3<float> dome_position;
    fpmed::Vec3<float> dome_rotation;
    fpmed::Vec3<float> dome_scale;

   public:
    Environment();
    ~Environment();

    // File functions
    void saveEnvironmentToFile(char *path);
    void loadEnvironmentFromFile(char *path);

    void setInfoName(char name[512]);
    void setManufacturerName(char name[512]);
    void setAuthorName(char name[512]);
    char *getInfoName();
    char *getManufacturerName();
    char *getAuthorName();
    int getNumberOfProjectors();

    // General functions for environment
    void pushProjector(Projector proj);
    void clearProjectors();
    Projector getProjector(int index);
    int countProjectors();

    void setProjectionType(int proj_t);
    void setConfiguration(int proj_c);
    void setScreenResolution(fpmed::Vec2<int> res);
    int getProjectionType();
    int getConfiguration();
    fpmed::Vec2<int> getScreenResolution();

    // Plnnar projection properties

    void setNumberOfScreenHorizontal(int n_screens);
    void setNumberOfScreensVertical(int n_screens);

    int getNumberOfScreenHorizontal();
    int getNumberOfScreensVertical();

    // Fulldome projection properties

    void setDomePosition(fpmed::Vec3<float> position);
    void setDomeRotation(fpmed::Vec3<float> rotation);
    void setDomeScale(fpmed::Vec3<float> scale);

    fpmed::Vec3<float> getDomePosition();
    fpmed::Vec3<float> getDomeRotation();
    fpmed::Vec3<float> getDomeScale();

    // digital theather location
    fpmed::Vec2<float> getLocation();
    void setLocation(fpmed::Vec2<float> loc);

    int getFullScreen();
    void setFullScreen(int val);
};
}  // namespace fpmed

#endif