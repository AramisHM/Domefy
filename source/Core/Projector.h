#ifndef PROJECTOR_H
#define PROJECTOR_H

#include <Math/vector4.h>
#include <Math/vector3.h>
#define name_len 512

namespace fpmed {

class Projector {
   private:
    // should be a unique id for the projector
    int index;
    float fov;
    float far_clip;

    fpmed::Vec3<float> offsetPosition;
    fpmed::Vec3<float> offsetRotation;

    char name[name_len];

    // Fulldome properties
    fpmed::Vec3<float> position;
    fpmed::Vec3<float> rotation;
    fpmed::Vec3<float> scale;

    // Planar properties
    fpmed::Vec4<int> Clip;  // screen clipping

    // Indicates the eyee to simulate stereoscopy
    char eye_side;  // values are either l, or r, both lowercased. "n"
                    // identifies none of them.

   public:
    Projector();
    ~Projector();

    void setPosition(fpmed::Vec3<float> position);
    void setRotation(fpmed::Vec3<float> rotation);
    void setScale(fpmed::Vec3<float> scale);
    void setFov(float fov);
    void setClip(fpmed::Vec4<int> clip);
    void setIndex(int idx);

    fpmed::Vec3<float> getPosition();
    fpmed::Vec3<float> getRotation();
    fpmed::Vec3<float> getScale();
    float getFov();
    fpmed::Vec4<int> getClip();
    int getIndex();

    void setName(char *pname);
    char *getName();

    // eye side, for stereoscopic features
    void setSterioscopicEye(char v_eye);  // possible values are: "l" "r" "n"
    char getSterioscopicEye();

    void setFarClip(float fc);
    float getFarClip();

    void setOffsetPosition(fpmed::Vec3<float> opos);
    fpmed::Vec3<float> getOffsetPosition();

    void setOffsetRotation(fpmed::Vec3<float> orot);
    fpmed::Vec3<float> getOffsetRotation();
};
}  // namespace fpmed

#endif
