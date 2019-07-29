#include <Application/MyCustomApplication.h>

#include <iostream>
#include <sstream>

// Our custom componenets
#include <Application/Components/AnatomicViewer/AnatomicViewer.h>
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>
#include <Application/Components/TVComponent/TVComponent.h>
#include <Application/Components/VHP/VHP.h>
#include <Core/ProgramConfig.h>

#include <Urho3D/AngelScript/APITemplates.h>

extern std::string commandString;  // main.cpp
MyCustomApplication *application;

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
    TVComponent::RegisterObject(context_);  // alternative way to do this

    // Register custom Urho3D componenets

    asIScriptEngine *engine = GetSubsystem<Script>()->GetScriptEngine();
    // VHP

    // TODO: move this registration to a separated file!!!
    RegisterComponent<VHP>(engine, "VHP");
    engine->RegisterObjectMethod("VHP", "void CreateModel(String&in)",
                                 asMETHOD(VHP, CreateModel), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetViewNodeReference(Node@+)",
                                 asMETHOD(VHP, SetViewNodeReference),
                                 asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP",
                                 "void SetSagitalCut(float, float, bool)",
                                 asMETHOD(VHP, SetSagitalCut), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetCoronalCut(float, float)",
                                 asMETHOD(VHP, SetCoronalCut), asCALL_THISCALL);
    engine->RegisterObjectMethod("VHP", "void SetAxialCut(float, float)",
                                 asMETHOD(VHP, SetAxialCut), asCALL_THISCALL);

    // Registers custom C++ class in AngelScript and pass a class instance
    // (singleton)
    ProgramConfig *config = ProgramConfig::GetInstance();
    engine->RegisterObjectType(
        "ProgramConfig", 0,
        asOBJ_REF);  // asOBJ_REF because you wanted a reference call
    engine->RegisterObjectBehaviour("ProgramConfig", asBEHAVE_ADDREF,
                                    "void f()", asMETHOD(ProgramConfig, AddRef),
                                    asCALL_THISCALL);
    engine->RegisterObjectBehaviour(
        "ProgramConfig", asBEHAVE_RELEASE, "void f()",
        asMETHOD(ProgramConfig, ReleaseRef), asCALL_THISCALL);
    engine->RegisterObjectMethod(
        "ProgramConfig", "String GetVHPFile()",
        asMETHOD(ProgramConfig, GetPathToCustomAssetsFolderURHO3D),
        asCALL_THISCALL);
    engine->RegisterGlobalProperty(
        "ProgramConfig progConf",
        config);  // the class instance must be a pointer reference.
#endif
}

MyCustomApplication::MyCustomApplication(Context *context) : Sample(context) {
    this->RegisterCustomScriptAPI();
}

void MyCustomApplication::CreateScene() {
    Camera *cameraComp = cameraNode_->GetComponent<Camera>();
    cameraComp->SetFov(85.0f);

    Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();
    Renderer *renderer = GetSubsystem<Renderer>();
    renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
    renderer->SetTextureAnisotropy(16);
    renderer->SetTextureQuality(QUALITY_MAX);

    // Singleton with configuration variables
    ProgramConfig *config = ProgramConfig::GetInstance();
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
    for (auto const &proj : projections) {
        Node *domeCamNode =
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
    Urho3D::ResourceCache *cache = GetSubsystem<Urho3D::ResourceCache>();

    // Node *videoNode = scene_->CreateChild("Video");
    // XMLFile *file = cache->GetResource<XMLFile>("Objects/TV.xml");
    // videoNode->LoadXML(file->GetRoot());
    // auto videoComp = videoNode->CreateComponent<TVComponent>();
    // videoComp->OpenFileName("./Data/Videos/test_video.ogv");
    // StaticModel *sm = videoComp->GetComponent<StaticModel>();
    // videoComp->SetOutputModel(sm);

    SubscribeToEvent(E_SCENEUPDATE,
                     URHO3D_HANDLER(MyCustomApplication, HandleUpdates));
    // create C++ app
    CreateScene();

    Sample::Start();

#ifdef fpmed_allow_scripted_application
    frameworkScriptInstance->CreateObject(
        cache->GetResource<ScriptFile>("Scripts/18_CharacterDemo2.as"),
        "Fpmed");
    frameworkScriptInstance->Execute("void FpmedStart()");
#endif
}

void MyCustomApplication::Stop() {
    if (level)
        delete level;  // Perform optional cleanup after main loop has
                       // terminated
}

void MyCustomApplication::HandleUpdates(StringHash eventType,
                                        VariantMap &eventData) {
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // external commands
    {
        std::vector<std::string> commandSplitted;
        commandSplitted = split(commandString, ';');
        int cmdLen = commandSplitted.size();

        if (cmdLen > 1) {
            if (!commandSplitted[0].compare(
                    std::string("CPP"))) {  // C++ exclusively
                // TODO: move all this block to a separated file!

                // The following VRTX and VRTXBATCH regards the manipulation
                // of point-to-point calibration on projector view
                if (!commandSplitted[1].compare(std::string("VRTX"))) {
                    float x, y;
                    int viewport = std::stoi(commandSplitted[2]);
                    int v = std::stoi(commandSplitted[3]);
                    x = std::stof(commandSplitted[4]);
                    y = std::stof(commandSplitted[5]);
                    AnimateVertex(viewport, v, x, y);
                }
                if (!commandSplitted[1].compare(std::string("VRTXBATCH"))) {
                    int viewport = std::stoi(commandSplitted[2]);
                    int qtty = std::stoi(commandSplitted[3]);

                    for (int i = 4; i < 4 + (qtty * 3); i += 3) {
                        float x, y;
                        int v = std::stoi(commandSplitted[i]);
                        x = std::stof(commandSplitted[i + 1]);
                        y = std::stof(commandSplitted[i + 2]);
                        AnimateVertex(viewport, v, x, y);
                    }
                }
                // General projector parameters
                if (!commandSplitted[1].compare(std::string("PROJPOS"))) {
                    int projector = std::stoi(commandSplitted[2]);

                    Vector3 postion(std::stof(commandSplitted[3]),
                                    std::stof(commandSplitted[4]),
                                    std::stof(commandSplitted[5]));

                    cameraNodeDomeList_[projector]->SetPosition(postion);
                }
                if (!commandSplitted[1].compare(std::string("PROJROT"))) {
                    int projector = std::stoi(commandSplitted[2]);

                    Quaternion rotation(std::stof(commandSplitted[3]),
                                        std::stof(commandSplitted[4]),
                                        std::stof(commandSplitted[5]));

                    cameraNodeDomeList_[projector]->SetWorldRotation(rotation);
                }
                if (!commandSplitted[1].compare(std::string("PROJFOV"))) {
                    int projector = std::stoi(commandSplitted[2]);
                    Camera *c =
                        cameraNodeDomeList_[projector]->GetComponent<Camera>();
                    c->SetFov(std::stof(commandSplitted[3]));
                }
                if (!commandSplitted[1].compare(std::string("DOMEGRID"))) {
                    int isEnabled = std::stoi(commandSplitted[2]);

                    for (auto dome : _virtualDomes) {
                        auto grid = dome->GetChild("DOME_GRID");
                        // TODO: aside from setting grid visible, make the
                        // actual dome a little transparent

                        Variant va;  // we will set the material color with this
                                     // variant
                        if (isEnabled == 1) {
                            va = Variant(Vector4(0.2f, 0.2f, 0.2f, 0.01f));
                            dome->SetEnabled(false);
                            grid->SetEnabled(true);
                        } else {
                            va = Variant(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                            grid->SetEnabled(false);
                            dome->SetEnabled(true);
                        }

                        // the virtual dome has 5 materials, one
                        // for each face
                        for (int i = 0; i < 5; ++i) {
                            auto domeMaterial =
                                dome->GetComponent<Urho3D::StaticModel>()
                                    ->GetMaterial(i);
                            domeMaterial->SetShaderParameter("MatDiffColor",
                                                             va);
                        }
                    }
                }
                if (!commandSplitted[1].compare(std::string("CALIROT"))) {
                    // Sets the calibration mesh rotation

                    int viewportIndex = std::stoi(commandSplitted[2]);

                    Quaternion rotation(std::stof(commandSplitted[3]),
                                        std::stof(commandSplitted[4]),
                                        std::stof(commandSplitted[5]));

                    _geometryCorrectionNodes[viewportIndex]->SetRotation(
                        rotation);
                }
                if (!commandSplitted[1].compare(std::string("AIDGRID"))) {
                    // Disables/Enables the image of the aproximated ideal
                    // calibration, used as reference to do our own calibration
                    int isEnabled = std::stoi(commandSplitted[2]);

                    for (auto node : _calibrationAidNodes) {
                        node->SetEnabled(isEnabled);
                    }
                }
            } else {  // forward to script instance
                // if (!cmd.compare(std::string("SCRIPT"))) {  // external
                // text Get the command from network, redirect to script,
                // execute it, and print the scrip's response.
                VariantVector parameters;
                VariantMap vm;
                VariantMap vm2;
                vm["CMD"] = Urho3D::String(
                    commandString
                        .c_str());     // let .as split the command string.
                parameters.Push(vm);   // function arguments
                parameters.Push(vm2);  // return

                frameworkScriptInstance->Execute(
                    "void DataGate(VariantMap, VariantMap&)",
                    parameters);  // Execute, second parameters is return
                                  // value

                // extract and print return from angelscript
                VariantMap retVM = parameters.Back().GetVariantMap();
                // printf("C++: %s", retVM["RET"].GetString().CString());
                //}
            }
        }
    }
    commandString = "";  // Must clean it.
}