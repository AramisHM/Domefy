//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include <iostream>
#include <sstream>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <SDL/SDL_log.h>

#include "CharacterDemo.h"
#include "simple_app.h"
#include "UCefApp.h"

#include <Urho3D/DebugNew.h>

#include <Core/ProgramConfig.h>
#include <FPMED.H>

extern std::string commandString; // main.cpp
extern std::string scriptPath;
//=============================================================================
//=============================================================================
// URHO3D_DEFINE_APPLICATION_MAIN(CharacterDemo)

//=============================================================================
//=============================================================================
CharacterDemo::CharacterDemo(Context* context) 
    : Sample(context)
    , uCefApp_(NULL)
    , cefAppCreatedOnce_(false)
{
    // CefExecuteProcess() needs to be call in the constructor, otherwise, 
    // you'll get multiple windows when using SDL
    CefMainArgs main_args(NULL);

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line and,
    // if this is a sub-process, executes the appropriate logic.
    int exit_code = CefExecuteProcess(main_args, NULL, NULL);
}

//=============================================================================
//=============================================================================
CharacterDemo::~CharacterDemo()
{
    if ( uCefApp_ )
    {
        uCefApp_->DestroyAppBrowser();
        uCefApp_ = NULL;
    }

    // calling CefShutdown() w/o having called CefInitialize() once 
    // causes exception due to no context created for cef
    if ( cefAppCreatedOnce_ )
    {
        CefShutdown();
        Time::Sleep(10);
    }
}

//=============================================================================
//=============================================================================
void CharacterDemo::Setup()
{
    engineParameters_["WindowTitle"]   = GetTypeName();
    engineParameters_["LogName"]       = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"]    = false;
    engineParameters_["Headless"]      = false;
    engineParameters_["WindowWidth"]   = 1280; 
    engineParameters_["WindowHeight"]  = 720;
    //engineParameters_["ResourcePaths"] = "Data;CoreData;";
}

//=============================================================================
//=============================================================================
void CharacterDemo::Start()
{
    Sample::CreateScene();
    // Execute base class startup
    Sample::Start();
    //if (touchEnabled_)
    //    touch_ = new Touch(context_, TOUCH_SENSITIVITY);

    

    // Create the controllable character
    //CreateCharacter();

    // Create the UI content
    //CreateInstructions();

    // Subscribe to necessary events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    //Sample::InitMouseMode(MM_RELATIVE);

    // Create static scene content
    CreateScene();

    // #ifdef fpmed_allow_scripted_application
    // Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();
    // frameworkScriptInstance->CreateObject(
    //     cache->GetResource<ScriptFile>(Urho3D::String(scriptPath.c_str())),
    //     "Fpmed");
    // frameworkScriptInstance->Execute("void FpmedStart()");
    // #endif
}

//=============================================================================
//=============================================================================
void CharacterDemo::CreateScene()
{
    Camera *cameraComp = cameraNode_->GetComponent<Camera>();
    cameraComp->SetFov(85.0f);

    Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();
    Renderer *renderer = GetSubsystem<Renderer>();
    renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
    renderer->SetTextureAnisotropy(16);
    renderer->SetTextureQuality(QUALITY_MAX);

    // Singleton with configuration variables
    fpmed::ProgramConfig *config = fpmed::ProgramConfig::GetInstance();
    std::list<Projection> projections = config->GetProjections();

    // viewports --------------------------------
    Graphics *graphics = GetSubsystem<Graphics>();
    renderer->SetNumViewports(config->GetLoadedProjectionsCount() + 1);

    Camera *mainCam = cameraNode_->GetComponent<Camera>();
    // mainCam->SetEnabled(false);
    SharedPtr<Viewport> mainViewport(new Viewport(context_, scene_, mainCam));
    renderer->SetViewport(0, mainViewport);

    // the custom projections
    int aux = 1;
    for (auto const &proj : projections)
    {
        Node *domeCamNode =
            CreateDomeCamera(proj); // last element from sceneDomeList_ is the
                                    // scene for this camera
        // TODO: make a function convert vec4 to Urho's rectangle
        int px, py, pdx, pdy;
        px = proj._viewport.getP();
        py = proj._viewport.getQ();
        pdx = proj._viewport.getR();
        pdy = proj._viewport.getS();
        IntRect viewRect = IntRect(px, py, px + pdx, py + pdy);
        SharedPtr<Viewport> tempViewport(
            new Viewport(context_, sceneMorphcorrList_.back(),
                         domeCamNode->GetComponent<Camera>(), viewRect));
        renderer->SetViewport(aux, tempViewport);
        ++aux;
    }

    // // Howdy, slides goes here partner.
    // this->slideComponent = cameraNode_->CreateComponent<Slide>();
    // slideComponent->CreateSlide(Urho3D::String("./presentation/set.xml"));
}

//=============================================================================
//=============================================================================
void CharacterDemo::CreateCharacter()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* objectNode = scene_->CreateChild("Jack");
    objectNode->SetPosition(Vector3(0.0f, 2.0f, 0.0f));

    // spin node
    Node* adjustNode = objectNode->CreateChild("AdjNode");
    //adjustNode->SetRotation( Quaternion(180, Vector3(0,1,0) ) );

    // Create the rendering component + animation controller
    AnimatedModel* object = adjustNode->CreateComponent<AnimatedModel>();
    object->SetModel(cache->GetResource<Model>("Models/Mutant/Mutant.mdl"));
    object->SetMaterial(cache->GetResource<Material>("Models/Mutant/Materials/mutant_M.xml"));
    object->SetCastShadows(true);
    AnimationController *animCtrl = adjustNode->CreateComponent<AnimationController>();
    animCtrl->PlayExclusive("Models/Mutant/Mutant_HipHop1.ani", 0, true, 0.2f);

    // Set the head bone for manual control
    //object->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;

    // Create rigidbody, and set non-zero mass so that the body becomes dynamic
    RigidBody* body = objectNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(1.0f);

    // Set zero angular factor so that physics doesn't turn the character on its own.
    // Instead we will control the character yaw manually
    body->SetAngularFactor(Vector3::ZERO);

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(COLLISION_ALWAYS);

    // Set a capsule shape for collision
    CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();
    shape->SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));

    // Create the character logic component, which takes care of steering the rigidbody
    // Remember it so that we can set the controls. Use a WeakPtr because the scene hierarchy already owns it
    // and keeps it alive as long as it's not removed from the hierarchy
    //character_ = objectNode->CreateComponent<Character>();
}

//=============================================================================
//=============================================================================
void CharacterDemo::CreateInstructions()
{
    Graphics* graphics = GetSubsystem<Graphics>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    
    // Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText(
        "Use WASD keys and mouse/touch to move\n\n"
        "press F5 to launch a browser\n"
    );
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    // The text has multiple rows. Center them in relation to each other
    instructionText->SetTextAlignment(HA_CENTER);
    
    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 4);

    // fps text
    // fpsText_  = ui->GetRoot()->CreateChild<Text>();
    // fpsText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    // fpsText_->SetPosition(graphics->GetWidth() - 100, 5);
    // fpsText_->SetColor(Color::YELLOW);

    UIElement* root = ui->GetRoot();
    XMLFile* uiStyle = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    // Set style to the UI root so that elements will inherit it
    root->SetDefaultStyle(uiStyle);

    Cursor* cursor = new Cursor(context_);
    cursor->SetName("Cursor");
    cursor->SetStyleAuto(uiStyle);
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);
    ui->SetCursor(cursor) ;

    cursor->SetVisible(true);
    GetSubsystem<Input>()->SetMouseVisible(true);
}

void CharacterDemo::SubscribeToEvents()
{
    // Subscribe to Update event for setting the character controls before physics simulation
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CharacterDemo, HandleUpdate));

    // Subscribe to PostUpdate event for updating the camera position after physics simulation
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(CharacterDemo, HandlePostUpdate));

    // Unsubscribe the SceneUpdate event from base class as the camera node is being controlled in HandlePostUpdate() in this sample
    UnsubscribeFromEvent(E_SCENEUPDATE);
}

void CharacterDemo::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    Input* input = GetSubsystem<Input>();

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    UI* ui = GetSubsystem<UI>();
    IntVector2 pos = ui->GetCursorPosition();

    // Check the cursor is visible and there is no UI element in front of the cursor
    if ( ui->GetCursor()->IsVisible() && !ui->GetElementAt(pos, true) )
    {
        // Move the camera, scale movement with time step
        MoveCamera(timeStep);
    }

    //*********************************************
    if (input->GetKeyPress(KEY_F6) && uCefApp_ == NULL)
    {
        uCefApp_ = new UCefApp(context_);
        uCefApp_->CreateAppBrowser();
        cefAppCreatedOnce_ = true;
    }

    // fps text
    // fpsCounter_++;

    // if ( timerFps_.GetMSec(false) > 1000 )
    // {
    //     fpsText_->SetText(String("fps: ") + String(fpsCounter_));
    //     fpsCounter_ = 0;
    //     timerFps_.Reset();
    // }
}

void CharacterDemo::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
}

void CharacterDemo::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 15.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    IntVector2 mouseMove = input->GetMouseMove();
    yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);

    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    // Use the Translate() function (default local space) to move relative to the node's orientation.
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

