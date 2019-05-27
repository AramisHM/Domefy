#ifndef MYCUSTOMAPPLICATION_H
#define MYCUSTOMAPPLICATION_H

#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>
#include <Application/Components/SlideAnimator/SlideAnimator.h>
#include <Application/Sample.h>
#include <Core/auxiliarAppFuncs.h>
#include <FPMED.H>
#include <Urho3D.h>
#include <Urho3DAll.h>

namespace Urho3D {
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
}  // namespace Urho3D

extern ServerMachine* machineMaster;

/*
        Custom application class definition, in this
        case, the framework for slide presentation in
        fulldome, my master's project */

#define N_SLICES 6000  // NUMBER OF SLICES FOR THE HUMAN VISIBLE PROJECT

class MyCustomApplication : public Sample {
    URHO3D_OBJECT(MyCustomApplication, Sample);

   public:
    MyCustomApplication(Context* context);
    void Start();

#ifdef fpmed_allow_cpp_application
    void CreateScene();
    void MoveCamera(float timeStep);
    void HandleUpdates(StringHash eventType, VariantMap& eventData);
    void updateCameraPosition();
    void updateRemoteControls();
#endif
   protected:
#ifdef fpmed_allow_cpp_application
    Urho3D::Text* debTex; /* this will be used to show the debug info of this
                     prototype app */
    Material* hologramMaterial;  // this is a pointer to the billboard material
                                 // that we will change the texture.
    Node* hologramNode;
    // Material* holoTextArray[399];  // this is the array of pre-loaded
    // materials that can be rapidly changed
    // Material* slidesMaterialArray[1024];  // this holds the individual slides
    // materials
    // Material* slicesMaterials[N_SLICE7S];  // this holds the individual
    // slices materials Node* interestPointTexts_[1024];      // the texts for
    // interest points

    StaticModel* hologramPlane;
    // Node* slideNode;
    // StaticModel* slideModel;  // the master-slide model reference, used for
    // swap slides on the fly
    // fpmed::Slides s;  // the slides per-se
    Slide* slideComponent;

    // SlideTransitionAnimatior*
    //     mainSlideAnimator;  // used to make little animation on slide
    //                         // transitions

    // The viewer node itself, contains a sub-node named "ViewerModel" with the
    // model
    // Node* viewerNode;
    GrabbableUI* viewerGrab;

    // coords X Y are used to move camera in the polar system rather than the
    // normal x y z
    bool polarCam;
    float coordX;
    float corodY;

    // coordinates for slide
    GrabbableUI* slideGrab;

    // float slideXDeg, slideYDeg;
    // float slideDistance;

    int xaccel;          // used to give that momentum effect
    int yaccel;          // used to give that momentum effect
    float polarRadius_;  // camera's polar radius
    // float loTransparency;
    float cameraNearClipping;
    // int accelDecayIteration;
#define accelDecayVar 25;
    bool isholding;  // holding mouse?
#endif               // fpmed_allow_cpp_application
};
#endif