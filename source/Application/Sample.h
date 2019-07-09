/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017
   Aramis Hornung Moraes See Fpmed.h for license and conditions for using this
   code
*/

#pragma once

#include "Urho3D/Engine/Application.h"

// scripts include
#include <Core/ProgramConfig.h>
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/AngelScript/ScriptInstance.h>

namespace Urho3D {
class Node;
class Scene;
class Sprite;
}  // namespace Urho3D

using namespace Urho3D;

class Sample : public Application {
    // Enable type information.
    URHO3D_OBJECT(Sample, Application);

   public:
    /// Construct.
    Sample(Context* context);

    /// Setup before engine initialization. Modifies the engine parameters.
    virtual void Setup();
    /// Setup after engine initialization. Creates the logo, console & debug
    /// HUD.
    virtual void Start();
    /// Cleanup after the main loop. Called by Application.
    virtual void Stop();

    int isApplication();
    void CloseApplication();
    // either use the Run() application, or the two below functions
    int Prepare();    // custom run methode
    int RunFrameC();  // custom update frame methode
    /// AUXILIAR CUSTOM FUNCTIONS ---------------------------------------

    /// Construct the scene content.
    void CreateScene();
    /// Set up viewport.
    void SetupViewport();
    /// Subscribe to update, UI and network events.
    void SubscribeToEvents();

   protected:
    /// Control logo visibility.
    void SetLogoVisible(bool enable);

    /// Logo sprite.
    SharedPtr<Sprite> logoSprite_;
    /// Scene.
    SharedPtr<Scene> scene_;
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;

    // Dome scenes, one for each viewport of fulldome projection we create.
    std::list<SharedPtr<Scene>> sceneDomeList_;
    // A list of "fisheye" cameras.
    std::list<SharedPtr<Node>> cameraNodeDomeList_;
    // Dome morphologic correction scenes.
    std::list<Scene*> sceneMorphcorrList_;
    // The morphologic correction camera node list.
    std::list<Node*> cameraNodeMorphcorrList_;
    // This is similar as above, but, instead of a node with 5 more nodes, this
    // is the camera with the component only.
    std::list<Node*> cameraNodeDomeAll_;
    // This vector holds the nodes that have the mesh for geometric correction
    std::vector<SharedPtr<VertexBuffer>> animatingBuffers_;

    SharedPtr<ScriptInstance> frameworkScriptInstance;

    // CreateDomeCamera - Creates the dome camera node.
    Node* CreateDomeCamera(Projection p);

   private:
    /// Create logo.
    void CreateLogo();
    /// Set custom window Title & Icon
    void SetWindowTitleAndIcon();
    /// Create console and debug HUD.
    void CreateConsoleAndDebugHud();
    /// Handle key down event to process key controls common to all samples.
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    /// Handle scene update event to control camera's pitch and yaw for all
    /// samples.
    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);

    /// Handle the logic post-update event.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle pressing the connect button.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
};
