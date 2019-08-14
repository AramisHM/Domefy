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

Sample::Sample(Context *context) : Application(context) { scene_ = 0; }

void Sample::Setup()
{
    fpmed::ProgramConfig *config = fpmed::ProgramConfig::GetInstance();

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
    engineParameters_["WindowPositionX"] = config->GetWindowPosition().getX();
    engineParameters_["WindowPositionY"] = config->GetWindowPosition().getY();
    engineParameters_["Monitor"] = config->GetMonitor();
}

void Sample::CreateLogo()
{
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    Texture2D *logoTexture =
        cache->GetResource<Texture2D>("fpmed/watermark_small.png");
    if (!logoTexture)
        return;

    UI *ui = GetSubsystem<UI>();
    logoSprite_ = ui->GetRoot()->CreateChild<Sprite>();
    logoSprite_->SetTexture(logoTexture);

    int textureWidth = logoTexture->GetWidth();
    int textureHeight = logoTexture->GetHeight();

    logoSprite_->SetScale(24.0f / textureWidth);
    logoSprite_->SetSize(textureWidth, textureHeight);
    logoSprite_->SetHotSpot(0, textureHeight);
    logoSprite_->SetAlignment(HA_LEFT, VA_BOTTOM);
    logoSprite_->SetOpacity(0.75f);
    logoSprite_->SetPriority(-100);
}

void Sample::Start()
{
#ifdef fpmed_allow_scripted_application

    // LOAD THE SCRIPTS - IF ANY
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    Sample::frameworkScriptInstance =
        scene_->CreateComponent<ScriptInstance>(LOCAL);

#endif

    CreateLogo();

    // Set custom window Title & Icon
    SetWindowTitleAndIcon();

    // Create console and debug HUD
    CreateConsoleAndDebugHud();

    // Subscribe key down event
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Sample, HandleKeyDown));

    // SetupViewport();
    SubscribeToEvents();
}
void Sample::Stop() { engine_->DumpResources(true); }

void Sample::CloseApplication() { engine_->Exit(); }
int Sample::isApplication()
{
    // printf("isapplication %d\n", (!engine_->IsExiting()) );
    return !engine_->IsExiting();
}
// auxiliar functions --------------------------

// extra functions --------------------------
int Sample::Prepare()
{
    // Emscripten-specific: C++ exceptions are turned off by default in -O1 (and
    // above), unless '-s DISABLE_EXCEPTION_CATCHING=0' flag is set Urho3D build
    // configuration uses -O3 (Release), -O2 (RelWithDebInfo), and -O0 (Debug)
    // Thus, the try-catch block below should be optimised out except in Debug
    // build configuration
    try
    {
        Setup();
        if (exitCode_)
            return exitCode_;

        if (engine_->Initialize(engineParameters_) == false)
        {
            ErrorExit();
            return exitCode_;
        }

        if (exitCode_)
            return exitCode_;
    }
    catch (std::bad_alloc &)
    {
        ErrorDialog(
            GetTypeName(),
            "An out-of-memory error occurred. The application will now exit.");
        return EXIT_FAILURE;
    }
}
int Sample::RunFrameC()
{
    // Emscripten-specific: C++ exceptions are turned off by default in -O1 (and
    // above), unless '-s DISABLE_EXCEPTION_CATCHING=0' flag is set
    // Urho3D build configuration uses -O3 (Release), -O2 (RelWithDebInfo), and
    // -O0 (Debug) Thus, the try-catch block below should be optimised out
    // except in Debug build configuration
    try
    {
        if (!engine_->IsExiting())
            engine_->RunFrame();
        // else
        // Stop();
        return exitCode_;
    }
    catch (std::bad_alloc &)
    {
        ErrorDialog(
            GetTypeName(),
            "An out-of-memory error occurred. The application will now exit.");
        return EXIT_FAILURE;
    }
}

void Sample::SetWindowTitleAndIcon()
{
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    Graphics *graphics = GetSubsystem<Graphics>();
    Image *icon = cache->GetResource<Image>("fpmed/domefy_logo256.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle("Domefy");
}
void Sample::CreateConsoleAndDebugHud()
{
    // Get default style
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    XMLFile *xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console *console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud *debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}

void Sample::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESCAPE)
    {
        Console *console = GetSubsystem<Console>();
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
}

// Create the main scene and camera, tjis scene is special because we simualte
// the fulldome and/or planar multiprojection view
void Sample::CreateScene()
{
    if (!scene_)
        scene_ = new Scene(context_);
    scene_->Clear();

    ResourceCache *cache = GetSubsystem<ResourceCache>();
    scene_->CreateComponent<Octree>(LOCAL);
    scene_->CreateComponent<PhysicsWorld>(LOCAL);

    cameraNode_ = scene_->CreateChild(
        "CameRef"); // criar uma camera normal para depois ser replicada
                    // pelos outros clientes, e com o dado nome para
                    // facilitar a referenciação desta mais tarde

    NetworkPriority *networkPriority =
        cameraNode_->CreateComponent<NetworkPriority>();
    networkPriority->SetBasePriority(100.0f);
    networkPriority->SetMinPriority(100.0f);
    networkPriority->SetAlwaysUpdateOwner(true);
    // Node* cameraNodeReference = cameraNode_->CreateChild("CameRef");

    Camera *camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(1024.0f);
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f)); // reset position
}

void Sample::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Sample, HandleUpdate));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Sample, HandlePostUpdate));
    GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTID);
}

void Sample::HandlePostUpdate(StringHash eventType, VariantMap &eventData) {}

void Sample::AnimateVertex(int mesh, int vertex, float x, float y)
{
    float startPhase = time_ + mesh * 30.0f;
    VertexBuffer *buffer = animatingBuffers_[mesh];

    // Lock the vertex buffer for update and rewrite positions with sine
    // wave modulated ones Cannot use discard lock as there is other
    // data (normals, UVs) that we are not overwriting
    unsigned char *vertexData =
        (unsigned char *)buffer->Lock(0, buffer->GetVertexCount());
    if (vertexData)
    {
        unsigned vertexSize = buffer->GetVertexSize();
        // unsigned numVertices = buffer->GetVertexCount();

        // If there are duplicate vertices, animate them in phase of
        // the original
        Vector3 &src = originalVertices_[vertex];
        Vector3 &dest =
            *reinterpret_cast<Vector3 *>(vertexData + vertex * vertexSize);
        // dest.x_ = src.x_ * (1.0f + 0.1f * Sin(phase));
        dest.y_ = src.y_ + (.00314f * y);
        dest.z_ = src.z_ + (.00314f * x);

        buffer->Unlock();
    }
}

void Sample::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    // Take the frame time step, which is stored as a float
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    // AnimateVertex(timeStep);
}

// Creates the camera that should be used by the fulldome viewports.
// It creates the scene the virtual dome, and on top of that, it creates another
// scene that holds a special mesh that we use to do a morphologic
// point-to-point geometry correction. Returns the node with the camera that
// view the final geometry compositions.
Node *Sample::CreateDomeCamera(Projection p)
{
    SharedPtr<Scene> sceneDome_;
    SharedPtr<Node> cameraNodeDome_;

    // create the scene that will hold the virtual dome
    ResourceCache *cache = GetSubsystem<ResourceCache>();
    sceneDome_ = new Scene(context_);
    sceneDome_->Clear();
    sceneDome_->CreateComponent<Octree>(LOCAL);
    Node *zoneNode = sceneDome_->CreateChild("Zone", LOCAL);
    Zone *zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetFogStart(250 / 4);
    zone->SetFogEnd(500);

    // create the virtual dome
    Node *domeNode = sceneDome_->CreateChild("VIRTUAL_DOME", LOCAL);
    StaticModel *domeMesh = domeNode->CreateComponent<StaticModel>();
    domeMesh->SetModel(cache->GetResource<Model>("Dome/Models/domeMesh.mdl"));
    domeNode->SetRotation(Urho3D::Quaternion(0.0f, p._domeYaw,0.0f));
    _virtualDomes.push_back(domeNode); // save for later referencing

    // create the grid for the virtual dome
    Node *domeGridNode = domeNode->CreateChild("DOME_GRID", LOCAL);
    StaticModel *domeGridMesh = domeGridNode->CreateComponent<StaticModel>();
    domeGridMesh->SetModel(
        cache->GetResource<Model>("Dome/Models/domeRuler.mdl"));
    domeGridMesh->SetMaterial(
        cache->GetResource<Material>("Dome/Materials/domegrid.xml"));
    // Invert the mesh
    domeGridNode->SetScale(
        Vector3(1.265f, -1.265f, 1.265f)); // this size is a magic number.
                                           // don't boder trying to understand.
    domeGridNode->Rotate(Quaternion(180.0f, 0.0f, 0.0f));
    domeGridNode->SetEnabled(false); // initially, it should be hidden

    // Create the camera that will see the vitual dome
    {
        cameraNodeDome_ = sceneDome_->CreateChild("CameraDome", LOCAL);
        Camera *cameraDome = cameraNodeDome_->CreateComponent<Camera>();
        cameraDome->SetFarClip(p._farClip);
        cameraDome->SetFov(p._fov);
        cameraNodeDome_->SetPosition(
            Vector3(p._projPos.getX(), p._projPos.getY(), p._projPos.getZ()));
        cameraNodeDome_->SetRotation(Quaternion(
            p._projRot.getX(), p._projRot.getY(), p._projRot.getZ()));
    }

    // creates the camera that lie within the real "application/game" scene,
    // with its position-offset and rotation-offset to simulate the stereoscopy.
    Node *cameraReferenceForDomeRender = 0;
    cameraReferenceForDomeRender =
        cameraNode_->CreateChild("DomeCamReference", LOCAL);
    cameraReferenceForDomeRender->SetPosition(
        Vector3(p._offsetPos.getX(), p._offsetPos.getY(), p._offsetPos.getZ()));
    cameraReferenceForDomeRender->SetRotation(Quaternion(
        p._offsetRot.getX(), p._offsetRot.getY(), p._offsetRot.getZ()));

    // Create the RTT's that we will place on the virtual dome mesh
    // up
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(p._rttResolution, p._rttResolution,
                                   Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface *surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node *cameraRttDomeNode_ =
            cameraReferenceForDomeRender->CreateChild("CameraUpNode_", LOCAL);
        Camera *camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
        camera->SetFarClip(p._farClip);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        cameraNodeDomeAll_.push_back(cameraRttDomeNode_);

        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(0, renderMaterial);
    }
    // right
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(p._rttResolution, p._rttResolution,
                                   Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface *surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node *cameraRttDomeNode_ = cameraReferenceForDomeRender->CreateChild(
            "CameraRightNode_", LOCAL);
        Camera *camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
        camera->SetFarClip(p._farClip);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        cameraNodeDomeAll_.push_back(cameraRttDomeNode_);

        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(1, renderMaterial);
    }
    // back
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(p._rttResolution, p._rttResolution,
                                   Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface *surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node *cameraRttDomeNode_ =
            cameraReferenceForDomeRender->CreateChild("CameraBackNode_", LOCAL);
        Camera *camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
        camera->SetFarClip(p._farClip);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        cameraNodeDomeAll_.push_back(cameraRttDomeNode_);

        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(2, renderMaterial);
    }
    // left
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(p._rttResolution, p._rttResolution,
                                   Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface *surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node *cameraRttDomeNode_ =
            cameraReferenceForDomeRender->CreateChild("CameraLeftNode_", LOCAL);
        Camera *camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 270.0f, 0.0f));
        camera->SetFarClip(p._farClip);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        cameraNodeDomeAll_.push_back(cameraRttDomeNode_);

        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(3, renderMaterial);
    }
    // front
    {
        SharedPtr<Texture2D> domeRenderTexture(new Texture2D(context_));
        domeRenderTexture->SetSize(p._rttResolution, p._rttResolution,
                                   Graphics::GetRGBFormat(),
                                   TEXTURE_RENDERTARGET);
        domeRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, domeRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface *surface = domeRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);
        Node *cameraRttDomeNode_ = cameraReferenceForDomeRender->CreateChild(
            "CameraFrontNode_", LOCAL);
        Camera *camera = cameraRttDomeNode_->CreateComponent<Camera>();
        cameraRttDomeNode_->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
        camera->SetFarClip(p._farClip);
        camera->SetAspectRatio(1.0f);
        camera->SetFov(90.0f);
        cameraNodeDomeAll_.push_back(cameraRttDomeNode_);

        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, scene_, cameraRttDomeNode_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        domeMesh->SetMaterial(4, renderMaterial);
    }

    // save the dome scene and camera-node references for later usage
    sceneDomeList_.push_back(sceneDome_);
    cameraNodeDomeList_.push_back(cameraNodeDome_);

    // ----------------------------------------------------------------------

    // Create the scene that will hold the deformable mesh for point-to-point
    // geometry correction
    // TODO: this block repeats too much, we should write a function for it.
    Node *retCam; // the camera node we are going to return
    {
        Scene *geometryCorrectionScene_ = new Scene(context_);
        geometryCorrectionScene_->Clear();
        geometryCorrectionScene_->CreateComponent<Octree>(LOCAL);
        Node *zoneNode = geometryCorrectionScene_->CreateChild("Zone", LOCAL);
        Zone *zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(255.0f, 0.0f, 255.0f));
        zone->SetFogColor(Color(255.0f, 0.08f, 0.08f));
        zone->SetFogStart(250 / 4);
        zone->SetFogEnd(500);
        sceneMorphcorrList_.push_back(geometryCorrectionScene_);

        Node *geometryCorrectionNode = geometryCorrectionScene_->CreateChild(
            "GEOMETRY_CORRECTION_NODE", LOCAL);
        StaticModel *geometryCorrectionMesh;

        // Get the original model and its unmodified vertices, which are used as
        // source data for the animation
        auto *originalModel =
            cache->GetResource<Model>("Models/geometryCorrection.mdl");
        if (!originalModel)
        {
            URHO3D_LOGERROR("Model not found, cannot initialize example scene");
            return NULL;
        }

        // Get the vertex buffer from the first geometry's first LOD level
        VertexBuffer *buffer =
            originalModel->GetGeometry(0, 0)->GetVertexBuffer(0);

        const auto *vertexData =
            (const unsigned char *)buffer->Lock(0, buffer->GetVertexCount());

        if (vertexData)
        {
            unsigned numVertices = buffer->GetVertexCount();
            unsigned vertexSize = buffer->GetVertexSize();

            // Copy the original vertex positions
            for (unsigned i = 0; i < numVertices; ++i)
            {
                const Vector3 &src = *reinterpret_cast<const Vector3 *>(
                    vertexData + i * vertexSize);
                originalVertices_.Push(src);
            }
            buffer->Unlock();

            // Detect duplicate vertices to allow seamless animation
            vertexDuplicates_.Resize(originalVertices_.Size());
            for (unsigned i = 0; i < originalVertices_.Size(); ++i)
            {
                vertexDuplicates_[i] = i; // Assume not a duplicate
                for (unsigned j = 0; j < i; ++j)
                {
                    if (originalVertices_[i].Equals(originalVertices_[j]))
                    {
                        vertexDuplicates_[i] = j;
                        break;
                    }
                }
            }
        }
        else
        {
            URHO3D_LOGERROR(
                "Failed to lock the model vertex buffer to get original "
                "vertices");
            return NULL;
        }

        // Create StaticModel in the scene. Clone the model so we
        // can modify the vertex data individually
        geometryCorrectionMesh =
            geometryCorrectionNode->CreateComponent<StaticModel>();
        SharedPtr<Model> cloneModel = originalModel->Clone();
        geometryCorrectionMesh->SetModel(cloneModel);
        geometryCorrectionNode->SetRotation(Quaternion(180.0f, -90.0f, 0.0f));
        _geometryCorrectionNodes.push_back(geometryCorrectionNode);

        // Store the cloned vertex buffer that we will modify when
        // animating
        animatingBuffers_.Push(SharedPtr<VertexBuffer>(
            cloneModel->GetGeometry(0, 0)->GetVertexBuffer(0)));

        // create rtt onto geometry correction mesh
        SharedPtr<Texture2D> geometryCorrectionRenderTexture(
            new Texture2D(context_));

        // TODO: this size shoulbe have its own config value on the
        // config.JSON
        geometryCorrectionRenderTexture->SetSize(
            p._viewport.getR(), p._viewport.getS(), Graphics::GetRGBFormat(),
            TEXTURE_RENDERTARGET);

        geometryCorrectionRenderTexture->SetFilterMode(FILTER_ANISOTROPIC);
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, geometryCorrectionRenderTexture);
        renderMaterial->SetDepthBias(BiasParameters(-0.0001f, 0.0f));
        RenderSurface *surface =
            geometryCorrectionRenderTexture->GetRenderSurface();
        surface->SetUpdateMode(SURFACE_UPDATEALWAYS);

        SharedPtr<Viewport> rttViewport(new Viewport(
            context_, sceneDome_, cameraNodeDome_->GetComponent<Camera>()));
        surface->SetViewport(0, rttViewport);

        geometryCorrectionMesh->SetMaterial(0, renderMaterial);
        {
            Node *geometryCorrCameraNode =
                geometryCorrectionScene_->CreateChild();
            Camera *geometryCorrCamera =
                geometryCorrCameraNode->CreateComponent<Camera>();
            geometryCorrCameraNode->SetPosition(Vector3(
                0, 0, -1.005f)); // Back off a bit, just to show the outline
                                 // created by the scene ambient color TODO:
                                 // should be parametrized and configurable on
                                 // the new calibrator

            geometryCorrCameraNode->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
            geometryCorrCamera->SetFarClip(p._farClip);
            geometryCorrCamera->SetAspectRatio(1.0f);
            geometryCorrCamera->SetFov(90.0f);
            cameraNodeMorphcorrList_.push_back(geometryCorrCameraNode);
            retCam = geometryCorrCameraNode;
        }

        // apply the morph correction loaded from config file
        for (auto v : p._morphMesh)
        {
            AnimateVertex(p._index, v.index, v.x, v.y);
        }

        // Add calibration aid mesh
        Node *calibReferenceNode = geometryCorrectionScene_->CreateChild(
            "CALIBRATION_REFERENCE", LOCAL);
        StaticModel *boxMesh =
            calibReferenceNode->CreateComponent<StaticModel>();
        boxMesh->SetModel(cache->GetResource<Model>("Models/Box.mdl"));

        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffAddAlpha.xml"));
        Urho3D::Texture2D *t = cache->GetResource<Urho3D::Texture2D>(
            Urho3D::String("./Dome/Aid/"
                           "NORTH.png"));
#ifdef FPMED_LATEST_URHO3D
        t->SetFilterMode(FILTER_NEAREST_ANISOTROPIC);
#else
        t->SetFilterMode(FILTER_NEAREST);
#endif
        t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
        t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
        t->SetNumLevels(1);
        m->SetTexture(TU_DIFFUSE, t);
        Variant va = Variant(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        m->SetShaderParameter("MatDiffColor", va);
        boxMesh->SetMaterial(m);
        _calibrationAidNodes.push_back(
            calibReferenceNode);               // save for later referencing
        calibReferenceNode->SetEnabled(false); // disabled by default
    }
    return retCam;
}