#include <Application/MyCustomApplication.h>

#include <iostream>
#include <sstream>

// Our custom componenets
#include <Application/Components/AnatomicViewer/AnatomicViewer.h>
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>
#include <Application/Components/VHP/VHP.h>
#include <Core/ProgramConfig.h>

#include <Urho3D/AngelScript/APITemplates.h>

extern std::string commandString;  // main.cpp
MyCustomApplication* application;

void MyCustomApplication::RegisterCustomScriptAPI() {
#ifdef fpmed_allow_scripted_application
    // Enable the script subsystem;
    context_->RegisterSubsystem(new Script(context_));

    // Register the Componenets to be usable on C++ level
    // Some of those componenets are not needed on script side.
    context_->RegisterFactory<VHP>();
    context_->RegisterFactory<GrabbableUI>();
    context_->RegisterFactory<Slide>();
    context_->RegisterFactory<AnatomicViewer>();
    context_->RegisterFactory<SlideTransitionAnimatior>();

    // Register custom Urho3D componenets

    asIScriptEngine* engine = GetSubsystem<Script>()->GetScriptEngine();
    // VHP
    RegisterComponent<VHP>(engine, "VHP");
    engine->RegisterObjectMethod("VHP", "void CreateModel(String&in)", asMETHOD(VHP, CreateModel), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetViewNodeReference(Node@+)", asMETHOD(VHP, SetViewNodeReference), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetSagitalCut(float, float, bool)", asMETHOD(VHP, SetSagitalCut), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetCoronalCut(float, float)", asMETHOD(VHP, SetCoronalCut), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetAxialCut(float, float)", asMETHOD(VHP, SetAxialCut), asCALL_THISCALL);

    // Registers custom C++ class in AngelScript and pass a class instance (singleton)
    ProgramConfig* config = ProgramConfig::GetInstance();
    engine->RegisterObjectType("ProgramConfig", 0, asOBJ_REF);  // asOBJ_REF because you wanted a reference call
    engine->RegisterObjectBehaviour("ProgramConfig", asBEHAVE_ADDREF, "void f()", asMETHOD(ProgramConfig, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("ProgramConfig", asBEHAVE_RELEASE, "void f()", asMETHOD(ProgramConfig, ReleaseRef), asCALL_THISCALL);
    engine->RegisterObjectMethod("ProgramConfig", "String GetVHPFile()", asMETHOD(ProgramConfig, GetPathToCustomAssetsFolderURHO3D), asCALL_THISCALL);
    engine->RegisterGlobalProperty("ProgramConfig progConf", config);  // the class instance must be a pointer reference.
#endif
}

MyCustomApplication::MyCustomApplication(Context* context) : Sample(context) {
    this->RegisterCustomScriptAPI();
}

void MyCustomApplication::CreateScene() {
    Camera* cameraComp = cameraNode_->GetComponent<Camera>();
    cameraComp->SetFov(85.0f);

    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
    renderer->SetTextureAnisotropy(16);
    renderer->SetTextureQuality(QUALITY_MAX);

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

    level = new CLevelData(context_);

    if (level->InitScene(scene_)) {
        level->InitTVComponentForSceneNode("TV");

    } else {
        engine_->Exit();
    }

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

void MyCustomApplication::Stop() {
    if (level) delete level;

    // Perform optional cleanup after main loop has terminated
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
        if (cmdLen > 1) {
            std::string cmd;  // cmd and parameters
            cmd = commandSplitted[0];

            //if (!cmd.compare(std::string("SCRIPT"))) {  // external text
            // Get the command from network, redirect to script, execute it, and print the scrip's response.
            VariantVector parameters;
            VariantMap vm;
            VariantMap vm2;
            vm["CMD"] = Urho3D::String(commandString.c_str());  // let .as split the command string.
            parameters.Push(vm);                                // function arguments
            parameters.Push(vm2);                               // return

            frameworkScriptInstance->Execute("void DataGate(VariantMap, VariantMap&)", parameters);  // Execute, second parameters is return value

            // extract and print return from angelscript
            VariantMap retVM = parameters.Back().GetVariantMap();
            //printf("C++: %s", retVM["RET"].GetString().CString());
            //}
        }
        commandString = "";  // Must clean it.
    }
}