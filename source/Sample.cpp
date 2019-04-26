// TODO: PARA CADA UMA DAS VARIAVEIS GLOBAIS ABAIXO MOVER ELAS PARA A CLASSE
// SAMPLE E CRIAR UMA MÉTODO
#include "Environment.h"
#include "FPMED.H"
#include "Projector.h"
#include "Urho3DAll.h"

#include "Sample.h"

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

// Control bits we define
static const unsigned CTRL_FORWARD = 1;
static const unsigned CTRL_BACK = 2;
static const unsigned CTRL_LEFT = 4;
static const unsigned CTRL_RIGHT = 8;

// DEFINE_APPLICATION_MAIN(Sample) // instead of it use the main(){}

#include "ProjectorMachine.h"
#include "ServerMachine.h"
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
    sceneDome_ = 0;
}

void Sample::Setup() {
    // Modify engine startup parameters
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["Fpmed-logfile"] =
        GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") +
        GetTypeName() + ".log";
    engineParameters_["FullScreen"] =
        false || (selected_serv == 0 && globalEnv.getFullScreen()) ||
        (selected_serv == 1 && machineMaster &&
         machineMaster->getServerProperties().fullScreen);
    engineParameters_["Headless"] = false;
    if (selected_serv == 0)  // é projetor
    {
        engineParameters_["WindowWidth"] =
            globalEnv.getScreenResolution().getX();
        engineParameters_["WindowHeight"] =
            globalEnv.getScreenResolution().getY();
    } else {
        engineParameters_["WindowWidth"] =
            machineMaster->getServerProperties().resolution.getX();
        engineParameters_["WindowHeight"] =
            machineMaster->getServerProperties().resolution.getY();
    }
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

    SetupViewport();
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

        if (!engine_->Initialize(engineParameters_)) {
            ErrorExit();
            fl_alert(
                "An error occurred right before trying to create 3D scene. Try "
                "checking your video drivers for compatibility.");

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
void Sample::loopCalibrateCamera() {
    if (selected_serv == 0)  // Projector
    {
        char* calibrationPacket;
        if (calibrationServ->update(1) == TEE_MESSAGE) {
            char *mode, *axis = 0;
            float calValue;

            Node* sceneCal = sceneDome_;
            Node* calNodeCam = sceneDome_->GetChild("CameraDome");
            Camera* calCam = calNodeCam->GetComponent<Camera>();
            Vector3 direction;
            // Mostra mensagem
            calibrationPacket = calibrationServ->getMessage();
            printf("CALIBRATION COMMAND: [%s]>: %s\n",
                   calibrationServ->getPacket().getSenderIP(),
                   calibrationPacket);

            mode = strtok(calibrationPacket, ";");

            if (!strcmp("rst", mode))  // not fov mode
            {
                // Reload the projector configuratiosn from the file
                {
                    fpmed::Vec3<float> projPos, projRot, projScale;

                    // le os parametros do projetor no arquivo de projetor
                    projPos =
                        globalEnv.getProjector(selected_proj).getPosition();
                    projRot =
                        globalEnv.getProjector(selected_proj).getRotation();
                    projScale =
                        globalEnv.getProjector(selected_proj).getScale();
                    calCam->SetFarClip(500.0f);
                    calCam->SetFov(
                        globalEnv.getProjector(selected_proj).getFov());
                    calNodeCam->SetPosition(Vector3(
                        projPos.getX(), projPos.getY(), projPos.getZ()));
                    calNodeCam->SetRotation(Quaternion(
                        projRot.getX(), projRot.getY(), projRot.getZ()));
                    calNodeCam->SetScale(Vector3(
                        projScale.getX(), projScale.getY(), projScale.getZ()));
                }
            } else if (!strcmp("dmp", mode))  // not fov mode
            {
                FILE* fp;
                fp = fopen("dumpDataCalibration.txt", "wb");

                Vector3 calCamP = calNodeCam->GetPosition();
                Quaternion calCamQ = calNodeCam->GetRotation();
                Vector3 calCamR = calCamQ.EulerAngles();
                fprintf(fp,
                        "<vector3d name=\"ProjectorPosition\" value=\"%f, %f, "
                        "%f\" />\n",
                        calCamP.x_, calCamP.y_, calCamP.z_);
                fprintf(fp,
                        "<vector3d name=\"ProjectorRotation\" value=\"%f, %f, "
                        "%f\" />\n",
                        calCamR.x_, calCamR.y_, calCamR.z_);
                fprintf(fp, "<float name=\"FOV\" value=\"%f\" />",
                        calCam->GetFov());

                fclose(fp);
            } else {
                if (strcmp("ccf", mode))  // not fov mode
                {
                    axis = strtok(NULL, ";");

                    if (!strcmp("x", axis))
                        direction =
                            -Vector3(0, 0, 1);  // hacked, dome is sideways
                    if (!strcmp("y", axis)) direction = Vector3(0, 1, 0);
                    if (!strcmp("z", axis))
                        direction =
                            Vector3(1, 0, 0);  // hacked, dome is sideways
                }
                calValue = atof(strtok(NULL, ";"));

                // Apply the calibration
                if (!strcmp("ccp", mode))  // apply translation
                {
                    direction = direction * calValue;
                    calNodeCam->SetPosition(
                        Vector3(calNodeCam->GetPosition() + direction));
                }
                if (!strcmp("ccr", mode))  // apply rotation
                {
                    direction = direction * calValue;
                    calNodeCam->Rotate(
                        Quaternion(direction.z_, direction.y_, -direction.x_));
                }
                if (!strcmp("ccf", mode))  // apply rotation
                {
                    calCam->SetFov(calCam->GetFov() + calValue);
                }
            }
        }
    }

    int auxiliarCommand = UpdateAuxiliarGlobalCommands();

    if (auxiliarCommand != 0)  // toogle grid on
    {
        char* enabled = strtok(NULL, ";");
        Node* gridDomeNode = sceneDome_->GetChild("VIRTUAL_DOME_GRID", LOCAL);

        if (auxiliarCommand == 55)
            gridDomeNode->SetEnabled(true);
        else if (auxiliarCommand == 56)
            gridDomeNode->SetEnabled(false);
    }
}
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

    if (selected_serv > 0)  // Servidor
    {
        printf("I'm a Server.\n");
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
    } else                                                    // Projetor
    {
        printf("\nI'm a Projector.\n");
        cameraNode_ = scene_->CreateChild(
            "Camera", LOCAL);  // cria localmente, pois nao pretendemos replicar
                               // essa camera

        // logica para offset posicao/rotacao
        // if we have any offset info.
        if (haveOffsetCamera(globalEnv.getProjector(
                selected_proj)))  // ESSE PROJETOR TEM OFFSET DE POSICAO OU
                                  // ROTACAO descrito no arquivo de mapeamento
                                  // da infraestrutura
        {
            fpmed::Vec3<float> offsetRot;
            fpmed::Vec3<float> offsetPos;

            offsetPos =
                globalEnv.getProjector(selected_proj).getOffsetPosition();
            offsetRot =
                globalEnv.getProjector(selected_proj).getOffsetRotation();

            Node* cameraNodeOfsset_ =
                cameraNode_->CreateChild("CameraOffset", LOCAL);

            Camera* camera = cameraNodeOfsset_->CreateComponent<Camera>();
            camera->SetFarClip(
                globalEnv.getProjector(selected_proj).getFarClip());

            cameraNodeOfsset_->SetPosition(
                Vector3(offsetPos.getX(), offsetPos.getY(), offsetPos.getZ()));
            cameraNodeOfsset_->SetRotation(Quaternion(
                offsetRot.getX(), offsetRot.getY(), offsetRot.getZ()));
            // we are using this offset camera node as the primary camera
            // node for rendering the viewport now. so we indicate this with the
            // "cameraNodeForComponentCamera", so later the code will give this
            // node the cmaera component
        } else {
            Camera* camera = cameraNode_->CreateComponent<Camera>();
            camera->SetFarClip(
                globalEnv.getProjector(selected_proj).getFarClip());
            cameraNode_->SetPosition(
                Vector3(0.0f, 0.0f, 0.0f));  // reset position
        }
    }

    int mxlen, mylen, n_mx, n_my;
    mxlen = globalEnv.getScreenResolution()
                .getX();  // len of x resolution of individual screens;
    mylen = globalEnv.getScreenResolution()
                .getY();  // len of y resolution of individual screens;
    n_mx = globalEnv.getNumberOfScreenHorizontal();  // n screens horizontal
    n_my = globalEnv.getNumberOfScreensVertical();   // n screens vertical
    {
        // Logica para segmentar as rtts em projeções palnas
        int scr_pos = 0;
        if (selected_proj != -1 &&
            selected_serv == 0)  // informacao de projecao planar, indice de 0-n
            scr_pos = globalEnv.getProjector(selected_proj).getIndex();
        if (selected_serv == 0 && scr_pos &&
            globalEnv.getProjectionType() == 1)  // proj plana
        {
            SharedPtr<Texture2D> renderTexture(new Texture2D(context_));
            renderTexture->SetSize(mxlen * n_mx, mylen * n_my,
                                   Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
            renderTexture->SetFilterMode(FILTER_ANISOTROPIC);
            Texture2D* rtt = renderTexture;
            UI* ui = GetSubsystem<UI>();

            int textureWidth = rtt->GetWidth() * n_mx;
            int textureHeight = rtt->GetHeight() * n_my;

            Sprite* rttSprite = ui->GetRoot()->CreateChild<Sprite>();
            rttSprite->SetTexture(rtt);

            rttSprite->SetHotSpot(0, 0);

            rttSprite->SetSize((mxlen * n_mx), (mylen * n_my));
            rttSprite->SetPriority(-100);

            int cp, lp;   // position to set the sprite; colun point, line point
            cp = lp = 0;  // initialize it
            int auxiliarIndextoXY =
                scr_pos;  // we gonna convert this index into x y coordinates
            for (lp = 0; lp < n_my; ++lp) {
                if (auxiliarIndextoXY <= 0) break;
                for (cp = 0; cp < n_mx; ++cp) {
                    if (auxiliarIndextoXY <= 0) break;
                    --auxiliarIndextoXY;
                }
            }

            // Set sprite alignment
            rttSprite->SetPosition(-mxlen * (cp - 1), -mylen * (lp - 1));

            RenderSurface* surface = renderTexture->GetRenderSurface();
            surface->SetUpdateMode(
                SURFACE_UPDATEALWAYS);  // update it even if it is not in the
                                        // view frustum!!

            if (haveOffsetCamera(globalEnv.getProjector(
                    selected_proj)))  // ESSE PROJETOR TEM OFFSET DE POSICAO OU
                                      // ROTACAO
            {
                SharedPtr<Viewport> rttViewport(
                    new Viewport(context_, scene_,
                                 cameraNode_->GetChild("CameraOffset")
                                     ->GetComponent<Camera>()));
                surface->SetViewport(0, rttViewport);
            } else {
                SharedPtr<Viewport> rttViewport(new Viewport(
                    context_, scene_, cameraNode_->GetComponent<Camera>()));
                surface->SetViewport(0, rttViewport);
            }
        }
    }

    // if it is a fulldome projection
    if (selected_serv == 0 &&
        globalEnv.getProjectionType() ==
            2)  // if == 2 then it is fulldome type  // proj fulldome
    {
        // Le as informações do arquivo de projetor e seta os valores das
        // variaveis
        fpmed::Vec3<float> posDome = globalEnv.getDomePosition();
        fpmed::Vec3<float> rotDome = globalEnv.getDomeRotation();
        fpmed::Vec3<float> scaleDome = globalEnv.getDomeScale();

        if (!sceneDome_) sceneDome_ = new Scene(context_);
        sceneDome_->Clear();
        sceneDome_->CreateComponent<Octree>(LOCAL);
        // sceneDome_->CreateComponent<PhysicsWorld>(LOCAL);

        Node* zoneNode = sceneDome_->CreateChild("Zone", LOCAL);
        Zone* zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        // zone->SetAmbientColor(Color(0.30f, 0.50f, 0.40f)); // TODO : remove
        // later
        zone->SetFogStart(globalEnv.getProjector(selected_proj).getFarClip() /
                          4);
        zone->SetFogEnd(globalEnv.getProjector(selected_proj).getFarClip());

        Node* domeNode = sceneDome_->CreateChild("VIRTUAL_DOME", LOCAL);
        domeNode->SetPosition(
            Vector3(posDome.getX(), posDome.getY(), posDome.getZ()));
        domeNode->SetScale(
            Vector3(scaleDome.getX(), scaleDome.getY(), scaleDome.getZ()));
        domeNode->SetRotation(
            Quaternion(rotDome.getX(), rotDome.getY(), rotDome.getZ()));
        StaticModel* domeMesh = domeNode->CreateComponent<StaticModel>();
        domeMesh->SetModel(
            cache->GetResource<Model>("Dome/Models/domeMesh.mdl"));

        // é o cliente domo (entao vamos criar a visao projetor dele)
        {
            fpmed::Vec3<float> projPos, projRot, projScale;

            // le os parametros do projetor no arquivo de projetor
            projPos = globalEnv.getProjector(selected_proj).getPosition();
            projRot = globalEnv.getProjector(selected_proj).getRotation();
            projScale = globalEnv.getProjector(selected_proj).getScale();

            cameraNodeDome_ = sceneDome_->CreateChild("CameraDome", LOCAL);
            Camera* cameraDome = cameraNodeDome_->CreateComponent<Camera>();
            cameraDome->SetFarClip(500.0f);
            cameraDome->SetFov(globalEnv.getProjector(selected_proj).getFov());
            cameraNodeDome_->SetPosition(
                Vector3(projPos.getX(), projPos.getY(), projPos.getZ()));
            cameraNodeDome_->SetRotation(
                Quaternion(projRot.getX(), projRot.getY(), projRot.getZ()));
            cameraNodeDome_->SetScale(
                Vector3(projScale.getX(), projScale.getY(), projScale.getZ()));
        }

        Node* cameraReferenceForDomeRender = 0;
        if (haveOffsetCamera(globalEnv.getProjector(selected_proj)))
            cameraReferenceForDomeRender =
                cameraNode_->GetChild("CameraOffset");
        else
            cameraReferenceForDomeRender = cameraNode_;

        // up
        {
            SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
            domeRenderTexture->SetSize(3000, 3000, Graphics::GetRGBFormat(),
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
                cameraReferenceForDomeRender->CreateChild("CameraUpNode_",
                                                          LOCAL);
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
            domeRenderTexture->SetSize(3000, 3000, Graphics::GetRGBFormat(),
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
                cameraReferenceForDomeRender->CreateChild("CameraRightNode_",
                                                          LOCAL);
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
            domeRenderTexture->SetSize(3000, 3000, Graphics::GetRGBFormat(),
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
                cameraReferenceForDomeRender->CreateChild("CameraBackNode_",
                                                          LOCAL);
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
            domeRenderTexture->SetSize(3000, 3000, Graphics::GetRGBFormat(),
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
                cameraReferenceForDomeRender->CreateChild("CameraLeftNode_",
                                                          LOCAL);
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
            domeRenderTexture->SetSize(3000, 3000, Graphics::GetRGBFormat(),
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
                cameraReferenceForDomeRender->CreateChild("CameraFrontNode_",
                                                          LOCAL);
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
        // cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

        // GRID DEBUG Dome (activated on .ini file)
        {
            Node* gridDomeNode =
                sceneDome_->CreateChild("VIRTUAL_DOME_GRID", LOCAL);
            gridDomeNode->SetPosition(
                Vector3(posDome.getX(), posDome.getY(), posDome.getZ()));
            gridDomeNode->SetScale(
                Vector3(scaleDome.getX(), scaleDome.getY(), scaleDome.getZ()));
            gridDomeNode->SetRotation(
                Quaternion(rotDome.getX(), rotDome.getY(), rotDome.getZ()));
            StaticModel* domeGridMesh =
                gridDomeNode->CreateComponent<StaticModel>();
            domeGridMesh->SetModel(
                cache->GetResource<Model>("Dome/Models/domeGridMesh.mdl"));
            domeGridMesh->SetMaterial(
                cache->GetResource<Material>("Dome/Materials/domegrid.xml"));
            gridDomeNode->SetEnabled(false);
            if (dome_grid) {
                gridDomeNode->SetEnabled(true);
            }
        }
    }
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

// Define qual camera ira ser mostrada na viewport
void Sample::SetupViewport() {
    Renderer* renderer = GetSubsystem<Renderer>();

    if (globalEnv.getProjectionType() == 2 &&
        selected_serv ==
            0)  // mostrar camera em perspectiva do domo, SOMOS PROJETOR
    {
        SharedPtr<Viewport> viewport(new Viewport(
            context_, sceneDome_, cameraNodeDome_->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);
    } else  // SOMOS SERVIDOR
    {
        SharedPtr<Viewport> viewport(new Viewport(
            context_, scene_,
            cameraNode_
                ->GetComponent<Camera>()));  // cameraNode_ camera component
        renderer->SetViewport(0, viewport);
    }
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
