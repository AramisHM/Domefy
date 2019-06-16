// TODO: PARA CADA UMA DAS VARIAVEIS GLOBAIS ABAIXO MOVER ELAS PARA A CLASSE
// SAMPLE E CRIAR UMA MÉTODO
#include <Core/Environment.h>
#include <Core/Projector.h>
#include <FPMED.H>
#include <Urho3DAll.h>

#include <Application/Sample.h>

#include <Core/ProgramConfig.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/NetworkPriority.h>
#include <Urho3D/Physics/PhysicsWorld.h>

static const unsigned short SERVER_PORT = defaultSceneRepPort;
// Identifier for our custom remote event we use to tell the client which object
// they control
static const Urho3D::StringHash E_CLIENTOBJECTID("ClientObjectID");
// Identifier for the node ID parameter in the event data
static const Urho3D::StringHash P_ID("ID");

// Control bits we define\
static const unsigned CTRL_FORWARD = 1;
static const unsigned CTRL_BACK = 2;
static const unsigned CTRL_LEFT = 4;
static const unsigned CTRL_RIGHT = 8;

// DEFINE_APPLICATION_MAIN(Sample) // instead of it use the main(){}

#include <Core/ProjectorMachine.h>
#include <Core/ServerMachine.h>
ServerMachine* machineMaster = 0;
ProjectorMachine* projectorMachine = 0;
TNETServer* ScriptServ =
    0;  // setup a simple udp server for receiving commands to script api

Sample::Sample(Context* context)
    : Application(context),
      yaw_(0.0f),
      pitch_(0.0f),
      touchEnabled_(false),
      screenJoystickIndex_(M_MAX_UNSIGNED),
      screenJoystickSettingsIndex_(M_MAX_UNSIGNED),
      paused_(false) {
    appHasStarted = false;
    scene_ = 0;
}

void Sample::Setup() {
    fpmed::ProgramConfig* config = fpmed::ProgramConfig::GetInstance();

    // Modify engine startup parameters
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["Fpmed-logfile"] =
        GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") +
        GetTypeName() + ".log";
    engineParameters_["FullScreen"] = config->IsFullscreen();
    engineParameters_["Headless"] = false;
    engineParameters_["Borderless"] = config->IsBorderless();
    engineParameters_["WindowWidth"] = config->GetWindowResolution().getX();
    engineParameters_["WindowHeight"] = config->GetWindowResolution().getY();
}

void Sample::CreateLogo() {
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D* logoTexture =
        cache->GetResource<Texture2D>("fpmed/watermark_small.png");
    if (!logoTexture) return;

    UI* ui = GetSubsystem<UI>();
    logoSprite_ = ui->GetRoot()->CreateChild<Sprite>();
    logoSprite_->SetTexture(logoTexture);

    int textureWidth = logoTexture->GetWidth();
    int textureHeight = logoTexture->GetHeight();

    logoSprite_->SetScale(256.0f / textureWidth);
    logoSprite_->SetSize(textureWidth, textureHeight);
    logoSprite_->SetHotSpot(0, textureHeight);
    logoSprite_->SetAlignment(HA_LEFT, VA_BOTTOM);
    logoSprite_->SetOpacity(0.75f);
    logoSprite_->SetPriority(-100);
}

void Sample::Start() {
    if (appHasStarted == true) return;
        // parse scripts if any

#ifdef fpmed_allow_scripted_application
    if (selected_serv > 0 &&
        run_scripts == 1)  // somente servidor carrega o script
    {
        // LOAD THE SCRIPTS - IF ANY
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        Sample::frameworkScriptInstance =
            scene_->CreateComponent<ScriptInstance>(LOCAL);

        Sample::frameworkScriptInstance->CreateObject(
            cache->GetResource<ScriptFile>(applicationScriptFileDirectory),
            "Fpmed");
        // Call the script object's "SetRotationSpeed" function. Function
        // arguments need to be passed in a VariantVector
        Sample::frameworkScriptInstance->Execute("void FpmedStart(int)");
        if (ScriptServ) {
            delete ScriptServ;
            ScriptServ = 0;
        }

        ScriptServ = new TNETServer(
            defaultScriptBackDoorServerPort);  // this is used to send custom
                                               // commands to scripts, its util
                                               // to create, for example, a xbox
                                               // control protocol, to remotly
                                               // send the joystick data to the
                                               // game running in the script
    }
#endif

    if (selected_serv == 1)  // only draw logo in server instance
        CreateLogo();

    if (selected_serv == 0)  // se for um projetor
        calibrationServ = new TNETServer(calibrationServerPort);

    // Set custom window Title & Icon
    SetWindowTitleAndIcon();

    // Create console and debug HUD
    CreateConsoleAndDebugHud();

    // Subscribe key down event
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Sample, HandleKeyDown));

    // SetupViewport();
    SubscribeToEvents();

    appHasStarted = true;  // lets make sure it starts only once
}
void Sample::Stop() { engine_->DumpResources(true); }

void Sample::handleIncomingNetworkScriptCommands() {
#ifdef fpmed_allow_scripted_application
    if (selected_serv > 0 &&
        run_scripts == 1)  // somente o master atenderá a comandos vindos para
                           // servidor de script
    {
        /* If we receive data into the script server, lets execute the proper
         * function that will process it*/
        if (ScriptServ->update(1) == TEE_MESSAGE) {
            VariantVector parameters;
            parameters.Push(
                Variant(ScriptServ->getMessage()));  // Add an int parameter
            printf("=====[%s]>: %s\n", ScriptServ->getPacket().getSenderIP(),
                   ScriptServ->getMessage());

            frameworkScriptInstance->Execute("void FpmedServerBus(String)",
                                             parameters);
        }
    }
#endif
}

// auxiliar functions --------------------------
// indicates if the projector have offset data from the original camera position
// or rotation
int haveOffsetCamera(fpmed::Projector proj) {
    fpmed::Vec3<float> offsetRot, offsetPos;
    offsetPos = proj.getOffsetPosition();
    offsetRot = proj.getOffsetRotation();

    if ((offsetPos.getX() || offsetPos.getY() || offsetPos.getZ()) ||
        (offsetRot.getX() || offsetRot.getY() || offsetRot.getZ()))
        return 1;
    else
        return 0;
}
int Sample::IsConnectedToServer() {
    Network* network = GetSubsystem<Network>();
    Connection* serverConnection = network->GetServerConnection();

    if (serverConnection)
        return 1;
    else
        return 0;
}
void Sample::ConnectToSceneServer(int port, char* ip) {
    Network* network = GetSubsystem<Network>();

    clientObjectID_ = 0;
    network->Connect(ip, port, scene_);
}
void Sample::StartSceneServer(int port) {
    Network* network = GetSubsystem<Network>();
    network->StartServer(port);
}
void Sample::CloseApplication() { engine_->Exit(); }
int Sample::isApplication() {
    // printf("isapplication %d\n", (!engine_->IsExiting()) );
    return !engine_->IsExiting();
}
// auxiliar functions --------------------------

// extra functions --------------------------
int Sample::Prepare() {
    // Emscripten-specific: C++ exceptions are turned off by default in -O1 (and
    // above), unless '-s DISABLE_EXCEPTION_CATCHING=0' flag is set Urho3D build
    // configuration uses -O3 (Release), -O2 (RelWithDebInfo), and -O0 (Debug)
    // Thus, the try-catch block below should be optimised out except in Debug
    // build configuration
    try {
        Setup();
        if (exitCode_) return exitCode_;

        if (engine_->Initialize(engineParameters_) == false) {
            ErrorExit();

            return exitCode_;
        }

        if (exitCode_) return exitCode_;
    } catch (std::bad_alloc&) {
        ErrorDialog(
            GetTypeName(),
            "An out-of-memory error occurred. The application will now exit.");
        return EXIT_FAILURE;
    }
}
int Sample::RunFrameC() {
    // Emscripten-specific: C++ exceptions are turned off by default in -O1 (and
    // above), unless '-s DISABLE_EXCEPTION_CATCHING=0' flag is set
    // Urho3D build configuration uses -O3 (Release), -O2 (RelWithDebInfo), and
    // -O0 (Debug) Thus, the try-catch block below should be optimised out
    // except in Debug build configuration
    try {
        if (!engine_->IsExiting()) engine_->RunFrame();
        // else
        // Stop();
        return exitCode_;
    } catch (std::bad_alloc&) {
        ErrorDialog(
            GetTypeName(),
            "An out-of-memory error occurred. The application will now exit.");
        return EXIT_FAILURE;
    }
}
void Sample::loopCalibrateCamera() {}
// extra functions --------------------------

void Sample::SetWindowTitleAndIcon() {
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Image* icon = cache->GetResource<Image>("fpmed/domefy_logo256.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle("Domefy");
}
void Sample::CreateConsoleAndDebugHud() {
    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}

void Sample::HandleKeyDown(StringHash eventType, VariantMap& eventData) {
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESCAPE) {
        Console* console = GetSubsystem<Console>();
        if (console->IsVisible())
            console->SetVisible(false);
        else
            engine_->Exit();
    }

    // Toggle console with F1
    else if (key == KEY_F1)
        GetSubsystem<Console>()->Toggle();

    // Toggle debug HUD with F2
    else if (key == KEY_F2)
        GetSubsystem<DebugHud>()->ToggleAll();

    // Toggle on/off domegrid
    // else if (key == KEY_F3) {
    //     if (selected_serv > 0)  // Servidor
    //         machineMaster->ToggleDomeGrid();

    // }

    // Common rendering quality controls, only when UI has no focused element
    else if (!GetSubsystem<UI>()->GetFocusElement()) {
        Renderer* renderer = GetSubsystem<Renderer>();

        // Texture quality
        if (key == '1') {
            int quality = renderer->GetTextureQuality();
            ++quality;
            if (quality > QUALITY_HIGH) quality = QUALITY_LOW;
            renderer->SetTextureQuality(quality);
        }

        // Material quality
        else if (key == '2') {
            int quality = renderer->GetMaterialQuality();
            ++quality;
            if (quality > QUALITY_HIGH) quality = QUALITY_LOW;
            renderer->SetMaterialQuality(quality);
        }

        // Specular lighting
        else if (key == '3')
            renderer->SetSpecularLighting(!renderer->GetSpecularLighting());

        // Shadow rendering
        else if (key == '4')
            renderer->SetDrawShadows(!renderer->GetDrawShadows());

        // Shadow map resolution
        else if (key == '5') {
            int shadowMapSize = renderer->GetShadowMapSize();
            shadowMapSize *= 2;
            if (shadowMapSize > 2048) shadowMapSize = 512;
            renderer->SetShadowMapSize(shadowMapSize);
        }

        // Shadow depth and filtering quality
        else if (key == '6') {
            ShadowQuality quality = renderer->GetShadowQuality();
            quality = (ShadowQuality)(quality + 1);
            if (quality > SHADOWQUALITY_BLUR_VSM)
                quality = SHADOWQUALITY_SIMPLE_16BIT;
            renderer->SetShadowQuality(quality);
        }

        // Occlusion culling
        else if (key == '7') {
            bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
            occlusion = !occlusion;
            renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
        }

        // Instancing
        else if (key == '8')
            renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());

        // Take screenshot
        else if (key == '9') {
            Graphics* graphics = GetSubsystem<Graphics>();
            Image screenshot(context_);
            graphics->TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            // screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() +
            // "Data/Screenshot_" + Time::GetTimeStamp().Replaced(':',
            // '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        }
    }
}

// Create the main scene and camera, tjis scene is special because we simualte
// the fulldome and/or planar multiprojection view
void Sample::CreateScene() {
    if (!scene_) scene_ = new Scene(context_);
    scene_->Clear();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    scene_->CreateComponent<Octree>(LOCAL);
    scene_->CreateComponent<PhysicsWorld>(LOCAL);

    cameraNode_ = scene_->CreateChild(
        "CameRef");  // criar uma camera normal para depois ser replicada
                     // pelos outros clientes, e com o dado nome para
                     // facilitar a referenciação desta mais tarde

    NetworkPriority* networkPriority =
        cameraNode_->CreateComponent<NetworkPriority>();
    networkPriority->SetBasePriority(100.0f);
    networkPriority->SetMinPriority(100.0f);
    networkPriority->SetAlwaysUpdateOwner(true);
    // Node* cameraNodeReference = cameraNode_->CreateChild("CameRef");

    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(1024.0f);
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));  // reset position
}

// create a standby scene for when we are with the projectors waiting for a
// server connection, at least display something for projection.
void Sample::createStandbyScene() {
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Zone* zone = scene_->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetAmbientColor(Color(0.5f, 0.5f, 0.5f));
    zone->SetFogColor(Color(0.18f, 0.08f, 0.08f));
    zone->SetFogStart(1.0f);
    zone->SetFogEnd(100.0f);

    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(300.0f, 1.0f, 300.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(
        cache->GetResource<Urho3D::Material>("Materials/BlackGrid.xml"));

    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(0.8f, 0.8f, 0.8f));

    cameraNode_ = scene_->GetChild("Camera");
    // cameraNode_->CreateComponent<Camera>();
    cameraNode_->SetPosition(Vector3(0.0f, 10.0f, -30.0f));

    Node* text3DNode = scene_->CreateChild("Text3D");
    text3DNode->SetPosition(Vector3(0.0f, 9.0f, 30.0f));
    Text3D* text3D = text3DNode->CreateComponent<Text3D>();

    // Manually set text in the current language.
    text3D->SetText(
        "\
       WELCOME TO FTD Digital Arena\n \
DOMEFY - Framework for Multi-Projection Systems\n \
            PROTOTYPE " +
        Urho3D::String(fpmed_version));

    text3D->SetFont(cache->GetResource<Urho3D::Font>("Fonts/Anonymous Pro.ttf"),
                    30);
    text3D->SetColor(Color::WHITE);
    text3D->SetAlignment(HA_CENTER, VA_CENTER);
    text3DNode->SetScale(18);
}

void Sample::SyncCameraPosRot() {
    Network* network = GetSubsystem<Network>();
    Connection* serverConnection = network->GetServerConnection();

    if (serverConnection)  // IM A CLIENT
    {
        // aside the scene, just copy the servers camera
        Node* serverCameraNode = 0;
        serverCameraNode = scene_->GetChild("CameRef");
        if (serverCameraNode) {
            cameraNode_->SetPosition(serverCameraNode->GetPosition());
            cameraNode_->SetRotation(serverCameraNode->GetRotation());
        }
    }
}

void Sample::SetupViewport() {
    // Renderer* renderer = GetSubsystem<Renderer>();

    // SharedPtr<Viewport> viewport(new Viewport(
    //     context_, scene_,
    //     cameraNode_->GetComponent<Camera>()));  // cameraNode_ camera
    //     component
    // renderer->SetViewport(0, viewport);
}
void Sample::SubscribeToEvents() {
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Sample, HandleUpdate));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Sample, HandlePostUpdate));
    // SubscribeToEvent(connectButton_, E_RELEASED, URHO3D_HANDLER(Sample,
    // HandleConnect));
    //    SubscribeToEvent(disconnectButton_, E_RELEASED, URHO3D_HANDLER(Sample,
    //    HandleDisconnect));
    // SubscribeToEvent(startServerButton_, E_RELEASED, URHO3D_HANDLER(Sample,
    // HandleStartServer)); SubscribeToEvent(E_SERVERCONNECTED,
    // URHO3D_HANDLER(Sample, HandleConnectionStatus));
    //  SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(Sample,
    //  HandleConnectionStatus));
    // SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(Sample,
    // HandleConnectionStatus));
    SubscribeToEvent(E_CLIENTCONNECTED,
                     URHO3D_HANDLER(Sample, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED,
                     URHO3D_HANDLER(Sample, HandleClientDisconnected));
    SubscribeToEvent(E_CLIENTOBJECTID,
                     URHO3D_HANDLER(Sample, HandleClientObjectID));
    GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTID);
}

void Sample::HandlePostUpdate(StringHash eventType, VariantMap& eventData) {
    SyncCameraPosRot();
}
void Sample::HandleUpdate(StringHash eventType, VariantMap& eventData) {
    handleIncomingNetworkScriptCommands();
}

void Sample::HandleDisconnect(StringHash eventType, VariantMap& eventData) {
    Network* network = GetSubsystem<Network>();
    Connection* serverConnection = network->GetServerConnection();

    if (serverConnection) {
        serverConnection->Disconnect();
        scene_->Clear(true, false);
        clientObjectID_ = 0;
    } else if (network->IsServerRunning()) {
        network->StopServer();
        scene_->Clear(true, false);
    }
}
void Sample::HandleStartServer(StringHash eventType, VariantMap& eventData) {
    Network* network = GetSubsystem<Network>();
    network->StartServer(SERVER_PORT);
}
void Sample::HandleConnectionStatus(StringHash eventType,
                                    VariantMap& eventData) {
    // UpdateButtons();
}
void Sample::HandleClientConnected(StringHash eventType,
                                   VariantMap& eventData) {
    using namespace ClientConnected;

    Connection* newConnection =
        static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    newConnection->SetScene(scene_);
}
void Sample::HandleClientDisconnected(StringHash eventType,
                                      VariantMap& eventData) {
    using namespace ClientConnected;

    Connection* connection =
        static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    Node* object = serverObjects_[connection];
    if (object) object->Remove();

    serverObjects_.Erase(connection);
}
void Sample::HandleClientObjectID(StringHash eventType, VariantMap& eventData) {
    clientObjectID_ = eventData[P_ID].GetUInt();
}

// TODO: this code is redundant, remove the old one that is based in the network
// replication.
// Returns the node with the dome camera node
Node* Sample::CreateDomeCamera(Projection p) {
    SharedPtr<Scene> sceneDome_;
    SharedPtr<Node> cameraNodeDome_;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    sceneDome_ = new Scene(context_);
    sceneDome_->Clear();
    sceneDome_->CreateComponent<Octree>(LOCAL);
    Node* zoneNode = sceneDome_->CreateChild("Zone", LOCAL);
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetFogStart(250 / 4);
    zone->SetFogEnd(500);
    Node* domeNode = sceneDome_->CreateChild("VIRTUAL_DOME", LOCAL);
    StaticModel* domeMesh = domeNode->CreateComponent<StaticModel>();
    domeMesh->SetModel(cache->GetResource<Model>("Dome/Models/domeMesh.mdl"));

    {
        fpmed::Vec3<float> projPos, projRot;

        cameraNodeDome_ = sceneDome_->CreateChild("CameraDome", LOCAL);
        Camera* cameraDome = cameraNodeDome_->CreateComponent<Camera>();
        cameraDome->SetFarClip(500.0f);
        cameraDome->SetFov(35.900139f);
        cameraNodeDome_->SetPosition(
            Vector3(p._projPos.getX(), p._projPos.getY(), p._projPos.getZ()));
        cameraNodeDome_->SetRotation(Quaternion(
            p._projRot.getX(), p._projRot.getY(), p._projRot.getZ()));
    }

    Node* cameraReferenceForDomeRender = 0;
    cameraReferenceForDomeRender = cameraNode_;

    // up
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(257, 257, Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface* surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node* cameraRttDomeNode_ =
            cameraReferenceForDomeRender->CreateChild("CameraUpNode_", LOCAL);
        Camera* camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
        camera->SetFarClip(500.0f);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(0, renderMaterial);
    }
    // right
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(257, 257, Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface* surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node* cameraRttDomeNode_ = cameraReferenceForDomeRender->CreateChild(
            "CameraRightNode_", LOCAL);
        Camera* camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
        camera->SetFarClip(500.0f);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(1, renderMaterial);
    }
    // back
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(257, 257, Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface* surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node* cameraRttDomeNode_ =
            cameraReferenceForDomeRender->CreateChild("CameraBackNode_", LOCAL);
        Camera* camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
        camera->SetFarClip(500.0f);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(2, renderMaterial);
    }
    // left
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(257, 257, Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface* surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node* cameraRttDomeNode_ =
            cameraReferenceForDomeRender->CreateChild("CameraLeftNode_", LOCAL);
        Camera* camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 270.0f, 0.0f));
        camera->SetFarClip(500.0f);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(3, renderMaterial);
    }
    // front
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(257, 257, Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface* surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node* cameraRttDomeNode_ = cameraReferenceForDomeRender->CreateChild(
            "CameraFrontNode_", LOCAL);
        Camera* camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
        camera->SetFarClip(500.0f);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(4, renderMaterial);
    }
    sceneDomeList_.push_back(sceneDome_);
    cameraNodeDomeList_.push_back(cameraNodeDome_);
    return cameraNodeDome_;
}