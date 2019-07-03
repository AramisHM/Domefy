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

    // replicate position and rotation of the master camera into the projector's
    // camera
    void SyncCameraPosRot();

    /// AUXILIAR CUSTOM FUNCTIONS ---------------------------------------
    // indicates if the application instance is  connected to a server (if is
    // replicating the scene) - Useful to know if a client is replicating a
    // scene
    int IsConnectedToServer();
    void StartSceneServer(int port);
    void ConnectToSceneServer(int port, char* ip);
    int isApplication();
    void CloseApplication();
    // either use the Run() application, or the two below functions
    int Prepare();    // custom run methode
    int RunFrameC();  // custom update frame methode
    /// AUXILIAR CUSTOM FUNCTIONS ---------------------------------------

    /// Listen to a port for external tools aiming the camera
    /// real-time-calibration. This function works best at the game-loop, in
    /// order to keep it alive, and listening. For calibrating, just send the
    /// right packets data to the designed port for this to work
    void loopCalibrateCamera();

    /// The default scene we load while we have nothing to replicate, at leaste
    /// we show something to be projected.
    void createStandbyScene();
    /// Construct the scene content.
    void CreateScene();
    /// Set up viewport.
    void SetupViewport();
    /// Subscribe to update, UI and network events.
    void SubscribeToEvents();

   protected:
    /// Return XML patch instructions for screen joystick layout for a specific
    /// sample app, if any.
    virtual String GetScreenJoystickPatchString() const {
        return String::EMPTY;
    }
    /// Initialize touch input on mobile platform.
    void InitTouchInput();
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

    /// Camera yaw angle.
    float yaw_;
    /// Camera pitch angle.
    float pitch_;
    /// Flag to indicate whether touch input has been enabled.
    bool touchEnabled_;

    SharedPtr<ScriptInstance> frameworkScriptInstance;

    bool appHasStarted;

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
    /// Handle touch begin event to initialize touch input on desktop platform.
    void HandleTouchBegin(StringHash eventType, VariantMap& eventData);
    void handleIncomingNetworkScriptCommands();  // passes theh commands that
                                                 // come from network to script
                                                 // application

    /// Screen joystick index for navigational controls (mobile platforms only).
    unsigned screenJoystickIndex_;
    /// Screen joystick index for settings (mobile platforms only).
    unsigned screenJoystickSettingsIndex_;
    /// Pause flag.
    bool paused_;

    /// Handle the logic post-update event.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle pressing the connect button.
    void HandleConnect(StringHash eventType, VariantMap& eventData);
    /// Handle pressing the disconnect button.
    void HandleDisconnect(StringHash eventType, VariantMap& eventData);
    /// Handle pressing the start server button.
    void HandleStartServer(StringHash eventType, VariantMap& eventData);
    /// Handle connection status change (just update the buttons that should be
    /// shown.)
    void HandleConnectionStatus(StringHash eventType, VariantMap& eventData);
    /// Handle a client connecting to the server.
    void HandleClientConnected(StringHash eventType, VariantMap& eventData);
    /// Handle a client disconnecting from the server.
    void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);
    /// Handle remote event from server which tells our controlled object node
    /// ID.
    void HandleClientObjectID(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    /// Mapping from client connections to controllable objects.
    HashMap<Connection*, WeakPtr<Node>> serverObjects_;
    unsigned clientObjectID_;
};
