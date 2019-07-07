#include <Application/MyCustomApplication.h>

#include <iostream>
#include <sstream>

// Our custom componenets
#include <Application/Components/AnatomicViewer/AnatomicViewer.h>
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>
#include <Application/Components/VHP/VHP.h>
#include <Core/ProgramConfig.h>

extern std::string commandString;  // main.cpp
MyCustomApplication* application;

MyCustomApplication::MyCustomApplication(Context* context) : Sample(context) {
#ifdef fpmed_allow_scripted_application
    context_->RegisterSubsystem(new Script(context_));
#endif
    context_->RegisterFactory<SlideTransitionAnimatior>();
    context_->RegisterFactory<GrabbableUI>();
    context_->RegisterFactory<VHP>();
    context_->RegisterFactory<Slide>();
    context_->RegisterFactory<AnatomicViewer>();
}

void MyCustomApplication::CreateScene() {
    Camera* cameraComp = cameraNode_->GetComponent<Camera>();
    cameraComp->SetFov(85.0f);

    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    // renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
    // renderer->SetTextureAnisotropy(16);
    // renderer->SetTextureQuality(QUALITY_MAX);

    // Hologram node - The actual custom code TODO: make componenet
    Node* orbitableNode = scene_->CreateChild("orbitableNode");
    orbitableNode->SetPosition(Vector3(0, 0, 0));
    cameraGrab = cameraNode_->CreateComponent<GrabbableUI>();
    cameraGrab->SetRotationOffset(Vector3(0.0f, -90.0f, 0));
    cameraGrab->SetOrbitableReference(orbitableNode);
    cameraGrab->SetRadiusAlterMoveSpeed(true);

    // Singleton with configuration variables
    ProgramConfig* config = ProgramConfig::GetInstance();
    std::list<Projection> projections = config->GetProjections();

    // TODO: Howdy, anatomic viewer
    anatomicViewer = cameraNode_->CreateComponent<AnatomicViewer>();
    anatomicViewer->CreateViewer();  // must be called

    // viewports --------------------------------
    Graphics* graphics = GetSubsystem<Graphics>();
    renderer->SetNumViewports(config->GetLoadedProjectionsCount() + 1);

    Camera* mainCam = cameraNode_->GetComponent<Camera>();
    // mainCam->SetEnabled(false);
    SharedPtr<Viewport> mainViewport(new Viewport(context_, scene_, mainCam));
    renderer->SetViewport(0, mainViewport);

    // the custom projections
    int aux = 1;
    for (auto const& proj : projections) {
        Node* domeCamNode =
            CreateDomeCamera(proj);  // last element from sceneDomeList_ is the
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
}

std::vector<std::string> split(std::string strToSplit, char delimeter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

void MyCustomApplication::Start() {
    // Execute base class startup
    Sample::CreateScene();  // create fulldome's scene
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    // debug text specific.
    {
        UI* ui = GetSubsystem<UI>();

        debTex = ui->GetRoot()->CreateChild<Urho3D::Text>();
        debTex->SetText("Loading images, please wait a moment.");
        debTex->SetFont(
            cache->GetResource<Urho3D::Font>("Fonts/Anonymous Pro.ttf"),
            15);
        debTex->SetTextAlignment(HA_CENTER);
        debTex->SetHorizontalAlignment(HA_CENTER);
        debTex->SetVerticalAlignment(VA_CENTER);
        debTex->SetPosition(0, ui->GetRoot()->GetHeight() / 4);
    }

    // load hologram textures
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        int seq = 8650;
    }

    SubscribeToEvent(E_SCENEUPDATE,
                     URHO3D_HANDLER(MyCustomApplication, HandleUpdates));
    // create C++ app
    CreateScene();

    Sample::Start();

#ifdef fpmed_allow_scripted_application
    frameworkScriptInstance->CreateObject(
        cache->GetResource<ScriptFile>("Scripts/18_CharacterDemo2.as"), "Fpmed");
    frameworkScriptInstance->Execute("void FpmedStart()");
#endif
}

void MyCustomApplication::HandleUpdates(StringHash eventType,
                                        VariantMap& eventData) {
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // external commands
    {
        std::vector<std::string> commandSplitted;
        commandSplitted = split(commandString, ';');
        int cmdLen = commandSplitted.size();
        if (cmdLen > 0) {
            if (cmdLen > 1) {
                std::string cmd;  // cmd and parameters
                cmd = commandSplitted[0];

                if (!cmd.compare(std::string("extext"))) {  // external text
                    //auxiliarText = commandSplitted[1]; // do something
                    // Get the command from network, redirect to script, execute it, and print the scrip's response.
                    VariantVector parameters;
                    VariantMap vm;
                    VariantMap vm2;
                    vm["TEXT"] = Urho3D::String(commandSplitted[1].c_str());
                    parameters.Push(vm);
                    parameters.Push(vm2);                                                                    // parameter
                                                                                                             // parameters.Push(VariantMap());                                              // return
                    frameworkScriptInstance->Execute("void DataGate(VariantMap, VariantMap&)", parameters);  // Execute, second parameters is return value
                    VariantMap retVM = parameters.Back().GetVariantMap();
                    printf("C++: %s", retVM["RET"].GetString().CString());
                }
            }
        }
    }
}