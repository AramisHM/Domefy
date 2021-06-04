#include <Application/MyCustomApplication.h>

#include <iostream>
#include <sstream>

#ifdef CEF_INTEGRATION
#include <Application/Components/WebBrowser/WebBrowser.h>
#include <Application/WebBrowser/CEFWebBrowser.h>
#endif
#include <Core/ProgramConfig.h>
#include <Urho3D/AngelScript/APITemplates.h>

extern std::string commandString;  // main.cpp
extern std::string scriptPath;
#ifdef CEF_INTEGRATION
extern std::string defaultCefUrl;
#endif
MyCustomApplication *application;
Slide *slidecomp;
bool cef_created = false;

void MyCustomApplication::RegisterCustomScriptAPI() {
#ifdef fpmed_allow_scripted_application

    // Enable the script subsystem;
    context_->RegisterSubsystem(new Script(context_));

    // Register the Componenets to be usable at C++ level
    // Some of those componenets are not needed on script side.
    context_->RegisterFactory<VHP>();
    context_->RegisterFactory<GrabbableUI>();
    context_->RegisterFactory<Slide>();
#ifdef CEF_INTEGRATION
    context_->RegisterFactory<WebBrowser>();
#endif
    // Register custom Urho3D componenets on scripting engine
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
    // Slides
    RegisterComponent<Slide>(engine, "Slide");
    engine->RegisterObjectMethod("Slide", "void CreateSlide(String&in)",
                                 asMETHOD(Slide, CreateSlide), asCALL_THISCALL);
    engine->RegisterObjectMethod("Slide", "void ApplyMouseMove(IntVector2&in)",
                                 asMETHOD(Slide, ApplyMouseMove),
                                 asCALL_THISCALL);
    engine->RegisterObjectMethod("Slide", "void SetCoordinates(IntVector2&in)",
                                 asMETHOD(Slide, SetCoordinates),
                                 asCALL_THISCALL);
    engine->RegisterObjectMethod("Slide", "void NextSlide()",
                                 asMETHOD(Slide, NextSlide), asCALL_THISCALL);
    engine->RegisterObjectMethod("Slide", "void PreviousSlide()",
                                 asMETHOD(Slide, PreviousSlide),
                                 asCALL_THISCALL);
    engine->RegisterObjectMethod("Slide", "void SetZoom(float)",
                                 asMETHOD(Slide, SetZoom), asCALL_THISCALL);

    engine->RegisterObjectMethod("Slide", "void MirrorSetZoom(float)",
                                 asMETHOD(Slide, MirrorSetZoom),
                                 asCALL_THISCALL);
    engine->RegisterObjectMethod(
        "Slide", "void MirrorApplyMouseMove(IntVector2&in)",
        asMETHOD(Slide, MirrorApplyMouseMove), asCALL_THISCALL);
    engine->RegisterObjectMethod(
        "Slide", "void MirrorSetCoordinates(IntVector2&in)",
        asMETHOD(Slide, MirrorSetCoordinates), asCALL_THISCALL);
    engine->RegisterObjectMethod("Slide", "void ToggleMirrorSlide()",
                                 asMETHOD(Slide, ToggleMirrorSlide),
                                 asCALL_THISCALL);

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

#ifdef CEF_INTEGRATION
    CEFWebBrowser *wb = CEFWebBrowser::GetInstance();
    wb->Init(context);
#endif
}

MyCustomApplication::~MyCustomApplication() {
#ifdef WIN32
#ifdef CEF_INTEGRATION
    CEFWebBrowser *wb = CEFWebBrowser::GetInstance();
    wb->ShutDown();
#endif
    ExitProcess(0);
#endif
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

    // slidecomp = cameraNode_->CreateComponent<Slide>();
    // slidecomp->CreateSlide("./presentation/set.xml");
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
    Sample::CreateScene();  // create fulldome's scene
    Sample::Start();
    Urho3D::ResourceCache *cache = GetSubsystem<Urho3D::ResourceCache>();
    SubscribeToEvent(E_SCENEUPDATE,
                     URHO3D_HANDLER(MyCustomApplication, HandleUpdates));
    CreateScene();  // create C++ app

#ifdef fpmed_allow_scripted_application
    Urho3D::ScriptFile *sf;
    sf = cache->GetResource<ScriptFile>(Urho3D::String(scriptPath.c_str()));
    frameworkScriptInstance->CreateObject(sf, "Fpmed");
    frameworkScriptInstance->Execute("void FpmedStart()");
#endif

#ifdef CEF_INTEGRATION
    Cursor *cursor = new Cursor(context_);
    Graphics *graphics = GetSubsystem<Graphics>();
    XMLFile *uiStyle = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    UI *ui = GetSubsystem<UI>();
    cursor->SetName("Cursor");
    cursor->SetStyleAuto(uiStyle);
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);
    ui->SetCursor(cursor);

    cursor->SetVisible(true);
    GetSubsystem<Input>()->SetMouseVisible(true);
#endif
}

void MyCustomApplication::Stop() {}

void MyCustomApplication::HandleUpdates(StringHash eventType,
                                        VariantMap &eventData) {
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    Input *input = GetSubsystem<Input>();

    if (input->GetKeyDown(KEY_F)) {
        IntVector2 md = input->GetMouseMove();
        // slidecomp->ApplyMouseMove(md);
#ifdef CEF_INTEGRATION
        webbrowser->ApplyMouseMove(md * 10);
#endif
    }
    if (input->GetKeyDown(KEY_H)) {
        IntVector2 md = input->GetMouseMove();
#ifdef CEF_INTEGRATION
        webbrowser->AddZoom(md.y_);
#endif
    }

    // external commands
    {
        std::vector<std::string> commandSplitted;
        commandSplitted = split(commandString, ';');
        int cmdLen = commandSplitted.size();

        if (cmdLen > 1) {
            for (int i = 0; i < commandSplitted.size(); ++i) {
                printf("\ncomand %s\n\n", commandSplitted[i].c_str());
            }

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
                if (!commandSplitted[1].compare(std::string("SLIDEMOVE"))) {
                    printf("%s", commandSplitted[1].c_str());
                    printf("%s", commandSplitted[2].c_str());
                    printf("%s", commandSplitted[3].c_str());
#ifdef CEF_INTEGRATION
                    // FIXME: dont use fix values
                    webbrowser->ApplyMouseMove(
                        IntVector2(std::stof(commandSplitted[2]) * 150.0f,
                                   std::stof(commandSplitted[3]) * 150.0f));
#endif
                }
                // set custom grid, passing file path
                if (!commandSplitted[1].compare(std::string("SETGRID"))) {
                    printf("%s", commandSplitted[1].c_str());

                    Urho3D::ResourceCache *cache =
                        GetSubsystem<ResourceCache>();

                    // create new material from scratch
                    SharedPtr<Material> m(new Material(context_));
                    m->SetTechnique(0, cache->GetResource<Technique>(
                                           "Techniques/DiffUnlit.xml"));
                    Urho3D::Texture2D *t =
                        cache->GetResource<Urho3D::Texture2D>(
                            Urho3D::String(commandSplitted[2].c_str()));
                    if (t) {
                        // t->SetFilterMode(FILTER_NEAREST);
                        t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
                        t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
                        t->SetNumLevels(1);
                        m->SetTexture(TU_DIFFUSE, t);
                        // m->SetShaderParameter("MatDiffColor", _modelColor);
                        // m->SetTechnique(0, cache->GetResource<Technique>(
                        //                        "Techniques/DiffAlphaTranslucent.xml"));
                        for (auto dome : _virtualDomes) {
                            auto grid = dome->GetChild("DOME_GRID");
                            Variant va;  // we will set the material color with
                                         // this variant

                            va = Variant(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                            m->SetShaderParameter("MatDiffColor", va);
                            grid->GetComponent<Urho3D::StaticModel>()
                                ->SetMaterial(m);
                        }
                    }
                }
#ifdef CEF_INTEGRATION

                if (!commandSplitted[1].compare(std::string("RUNURL"))) {
                    std::string url = commandSplitted[2];
                    if (url.length() > 3) {
                        webbrowser->LoadURL(url);
                    }
                }
                if (!commandSplitted[1].compare(std::string("STOPCEFMEDIA"))) {
                    webbrowser->DefaultURL();
                }
                if (!commandSplitted[1].compare(std::string("SLIDEZOOM"))) {
                    printf("%s", commandSplitted[1].c_str());
                    printf("%s", commandSplitted[2].c_str());

                    float radius = std::stof(commandSplitted[2]) / 5.0f;
                    webbrowser->SetZoom(radius);

                    fpmed::Vec2<float> coords =
                        webbrowser->GetGrabbableUI()->GetCoordinates();
                    printf("\n\n%f  - <%f, %f>", radius, coords.getX(),
                           coords.getY());
                }

                // USER360BROWSER
                if (!commandSplitted[1].compare(
                        std::string("USER360BROWSER"))) {
                    printf("%s", commandSplitted[2].c_str());

                    if (!commandSplitted[2].compare(std::string("1"))) {
                        webbrowser->SetSphericView(true);
                    } else {
                        webbrowser->SetSphericView(false);
                    }
                }

                // USER360CUBEBROWSER
                if (!commandSplitted[1].compare(
                        std::string("USER360CUBEBROWSER"))) {
                    printf("%s", commandSplitted[2].c_str());

                    if (!commandSplitted[2].compare(std::string("1"))) {
                        webbrowser->SetCubeView(true);
                    } else {
                        webbrowser->SetCubeView(false);
                    }
                }
                // USER360CUBE3DBROWSER
                if (!commandSplitted[1].compare(
                        std::string("USER3603DCUBEBROWSER"))) {
                    printf("%s", commandSplitted[2].c_str());

                    if (!commandSplitted[2].compare(std::string("1"))) {
                        webbrowser->SetCube3DView(true);
                    } else {
                        webbrowser->SetCube3DView(false);
                    }
                }
                // center browser
                if (!commandSplitted[1].compare(std::string("CENTERBROWSER"))) {
                    printf("%s", commandSplitted[2].c_str());

                    webbrowser->CenterBrowser();
                }

#endif

            } else {  // forward to script instance
                // if (!cmd.compare(std::string("SCRIPT"))) p{  // external
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

#ifdef CEF_INTEGRATION
    if (cef_created == false) {
        // second browser
        Node *browserNode = cameraNode_->CreateChild("browserNode");
        webbrowser = browserNode->CreateComponent<WebBrowser>();
        webbrowser->CreateWebBrowser();
        cef_created = true;
        // set initial position and coordinates
        webbrowser->GetGrabbableUI()->SetCoordinates(
            fpmed::Vec2<float>(263.19f, 22.12f));
        webbrowser->SetZoom(2.8f);
        // 183.600000  - <270.265472, 17.580265>
    }

    if (input->GetKeyPress(KEY_F6) && cef_created == true) {
        webbrowser->LoadURL(defaultCefUrl);
    }

    if (input->GetKeyPress(KEY_F8) && cef_created == true) {
        webbrowser->ToggleUIRender();
    }
#endif
}