#include <Application/MyCustomApplication.h>

#include <iostream>
#include <sstream>

// Our custom componenets
#include <Application/Components/AnatomicViewer/AnatomicViewer.h>
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>
#include <Application/Components/VHP/VHP.h>  // creates the vhp model
#include <Core/ProgramConfig.h>

extern std::string commandString;  // main.cpp
std::string auxiliarText;

MyCustomApplication* application;

MyCustomApplication::MyCustomApplication(Context* context) : Sample(context) {
// ENABLE SCRIPTS
#ifdef fpmed_allow_scripted_application
    context_->RegisterSubsystem(new Script(context_));
#endif

#ifdef fpmed_allow_cpp_application

    // Register the object factories
    context_->RegisterFactory<SlideTransitionAnimatior>();
    context_->RegisterFactory<GrabbableUI>();
    context_->RegisterFactory<VHP>();
    context_->RegisterFactory<Slide>();
    context_->RegisterFactory<AnatomicViewer>();
#endif
}

// Wrap the update camera function from the custom class app, to a
// regular function, so it can be called by the animator component
// (used to animate the camera when chaging slides).
void cameraCallback() { application->updateCameraPosition(); }

// makeNodeFaceCamera -
void makeNodeFaceCamera(Urho3D::Node* node, Urho3D::Camera* camComp) {
    using namespace std;
    Quaternion q = camComp->GetFaceCameraRotation(
        node->GetWorldPosition(),
        node->GetWorldRotation() + Quaternion(0.0f, 90.0f, 0.0f),
        FC_ROTATE_XYZ);
    Vector3 v = q.EulerAngles();
    node->SetRotation(q);
    // correct node side at your will
    node->Rotate(Quaternion(-90.0f, 0.0f, 0.0f));
}

#ifdef fpmed_allow_cpp_application
// Cpp implementation of a static scene for the application
void MyCustomApplication::CreateScene() {
    // adjust camera fov
    Camera* cameraComp = cameraNode_->GetComponent<Camera>();
    cameraComp->SetFov(85.0f);

    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    // renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
    // renderer->SetTextureAnisotropy(16);
    // renderer->SetTextureQuality(QUALITY_MAX);

    // Zone + lighting + fog
    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));
    zone->SetFogColor(Color(0.85f, 0.85f, 0.85f));
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    // north plane
    {
        Node* floorNode = scene_->CreateChild("FloorTile");
        floorNode->SetPosition(Vector3(0.0f, 0.0f, 30.0f));
        floorNode->SetScale(Vector3(15.0f, 1.0f, 15.0f));
        StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        floorObject->SetMaterial(
            cache->GetResource<Material>("Materials/Stone.xml"));
    }

    // west plane
    {
        Node* floorNode = scene_->CreateChild("FloorTile");
        floorNode->SetPosition(Vector3(30.0f, 0.0f, 0.0f));
        floorNode->SetScale(Vector3(4.0f, 1.0f, 4.f));
        StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        floorObject->SetMaterial(
            cache->GetResource<Material>("Materials/Stone.xml"));
    }

    // skyboxskybox
    // Node* skyNode = scene_->CreateChild("Sky");
    // Skybox* skybox = skyNode->CreateComponent<Skybox>();
    // skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    // skybox->SetMaterial(
    //     cache->GetResource<Material>("Materials/CustomSkybox.xml"));

    // Some random mesh for testing custom componenets
    {
        Node* componentNodeTest = scene_->CreateChild("TestComponent");
        componentNodeTest->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
        StaticModel* floorObject =
            componentNodeTest->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>(
            "Models/DoubleSidedMirroredPlaneRef.mdl"));
        floorObject->SetMaterial(
            cache->GetResource<Material>("Materials/Stone.xml"));
    }

    // Hologram node - The actual custom code TODO: make componenet
    hologramNode = scene_->CreateChild("Hologram");
    hologramNode->SetPosition(Vector3(0, 0, 0));
    hologramPlane = hologramNode->CreateComponent<StaticModel>();
    hologramPlane->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    hologramPlane->SetMaterial(
        cache->GetResource<Material>("Materials/transparent.xml"));
    hologramMaterial = hologramPlane->GetMaterial();
    hologramMaterial->SetShaderParameter("MatDiffColor",
                                         Urho3D::Color(.0f, .0f, .0f, 0.0f));
    hologramNode->SetRotation(Quaternion(-90, 180, 0));
    hologramNode->SetScale(Vector3(19.50f, 10.0f, 12.60f));

    cameraGrab = cameraNode_->CreateComponent<GrabbableUI>();
    cameraGrab->SetRotationOffset(Vector3(0.0f, -90.0f, 0));
    cameraGrab->SetOrbitableReference(hologramNode);
    // movement arround is not affected by how close it is to the
    // orbitable object
    cameraGrab->SetRadiusAlterMoveSpeed(false);

    // Singleton with configuration variables
    ProgramConfig* config = ProgramConfig::GetInstance();
    std::list<Projection> projections = config->GetProjections();

    // Howdy, VHP
    Urho3D::Node* vhpNode = scene_->CreateChild("VHP");
    vhp = vhpNode->CreateComponent<VHP>();
    try {
        vhp->CreateModel(config->GetPathToCustomAssetsFolder());
    } catch (const std::exception& e) {
        printf("Could not correctly load the VHP model");
    }

    vhp->SetViewNodeReference(cameraNode_);
    vhpNode->SetScale(15.0f);
    vhpNode->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
    vhpNode->SetRotation(Quaternion(90.0f, 0.0f, 0.0f));

    // Howdy, slides goes here partner.
    this->slideComponent = cameraNode_->CreateComponent<Slide>();
    slideComponent->CreateSlide(Urho3D::String("./presentation/set.xml"));

    // TODO: Howdy, anatomic viewer
    anatomicViewer = cameraNode_->CreateComponent<AnatomicViewer>();
    anatomicViewer->CreateViewer();  // must be called

    cameraNearClipping = 0.01f;

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
#endif

// TODO: move to a class
std::vector<std::string> split(std::string strToSplit, char delimeter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter)) {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

/* Moves the camera arrount */
#ifdef fpmed_allow_cpp_application
void MyCustomApplication::MoveCamera(float timeStep) {
    {  // external commands
        std::vector<std::string> commandSplitted;
        commandSplitted = split(commandString, ';');

        int cmdLen = commandSplitted.size();
        if (cmdLen > 0) {
            // printf("command recvd: %s\n", commandSplitted[0].c_str());

            if (cmdLen > 1) {
                std::string cmd, pa, pb, pc, pd;  // cmd and parameters
                cmd = commandSplitted[0];

                if (!cmd.compare(std::string("extext"))) {  // external text
                    auxiliarText = commandSplitted[1];
                }
                if (!cmd.compare(std::string("sag-a"))) {
                    double factor = std::atof(commandSplitted[1].c_str());
                    // printf("\nconta: %f\n", (float)(factor / 100.0f));
                    this->vhp->SetSagitalCut((float)(factor / 100.0f), 0.0f,
                                             true);
                }
                if (!cmd.compare(std::string("cor-a"))) {
                    double factor = std::atof(commandSplitted[1].c_str());
                    this->vhp->SetCoronalCut((float)(factor / 100.0f), 0.0f);
                }
                if (!cmd.compare(std::string("axi-a"))) {
                    double factor = std::atof(commandSplitted[1].c_str());
                    this->vhp->SetAxialCut((float)(factor / 100.0f), 0.0f);
                }
                if (!cmd.compare(std::string("obliq-a"))) {
                    double factor = std::atof(commandSplitted[1].c_str());
                    float nearClip;
                    for (std::list<Node*>::iterator it =
                             cameraNodeDomeAll_.begin();
                         it != cameraNodeDomeAll_.end(); ++it) {
                        Camera* camComp = (*it)->GetComponent<Camera>();

                        nearClip = ((factor / 100.0f) * 10.0f);
                        camComp->SetNearClip(nearClip);
                        Camera* mainCamComp =
                            cameraNode_->GetComponent<Camera>();
                        mainCamComp->SetNearClip(nearClip);
                    }
                }
                if (!cmd.compare(std::string("trans"))) {
                    double factor = std::atof(commandSplitted[1].c_str());
                    // printf("\nconta: %f\n", (float)(factor / 100.0f));
                    this->vhp->SetModelTransparency((float)(factor / 100.0f));
                }

                // Moves camera arround, given the custom js based viewer
                if (!cmd.compare(std::string("camparams"))) {
                    double p = std::atof(commandSplitted[1].c_str());
                    double q = std::atof(commandSplitted[2].c_str());
                    double x = std::atof(commandSplitted[3].c_str()) * 2;
                    double y = std::atof(commandSplitted[4].c_str()) * 2;
                    double z = std::atof(commandSplitted[5].c_str()) * 2;
                    double r = std::atof(commandSplitted[6].c_str());
                    cameraGrab->SetCoordinates(
                        fpmed::Vec2<float>((float)(p), (float)(q)));
                    hologramNode->SetPosition(Vector3(-y, z, x));
                    cameraGrab->SetRadius(r);
                }
            }
        }
    }

    UI* ui = GetSubsystem<UI>();
    Input* input = GetSubsystem<Input>();

    input->SetMouseVisible(true);

    // Do not move if the UI has a focused element (the console)
    if (ui->GetFocusElement()) return;

    // Movement speed as world units per second
    const float MOVE_SPEED = 5.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.015f;

    IntVector2 mouseMove = input->GetMouseMove();

    if (input->GetMouseButtonDown(
            MOUSEB_RIGHT)) {  // move either by momentum or mouse iteraction
        // camera momentum
        IntVector2 md = input->GetMouseMove();
        cameraGrab->SetMomentum(Vec2<float>(md.x_ / 5, md.y_ / 5));
        cameraGrab->ApplyMouseMove(Vec2<int>(md.x_, -md.y_));
    }

    if (input->GetKeyDown(KEY_F)) {
        IntVector2 md = input->GetMouseMove();
        slideComponent->ApplyMouseMove(Vec2<int>(md.x_, md.y_));
    }
    if (input->GetKeyDown(KEY_J)) {
        IntVector2 md = input->GetMouseMove();
        anatomicViewer->ApplyMouseMove(Vec2<int>(md.x_, md.y_));
    }

    // Read WASD keys and move the camera scene node to the corresponding
    // direction if they are pressed
    if (input->GetKeyDown(KEY_W)) {
        // cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
        hologramNode->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    }

    if (input->GetKeyDown(KEY_S)) {
        // cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
        hologramNode->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    }

    if (input->GetKeyDown(KEY_A)) {
        // cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
        hologramNode->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    }

    if (input->GetKeyDown(KEY_D)) {
        hologramNode->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
        // cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
    }

    if (input->GetKeyDown(KEY_E)) {
        // cameraNode_->Translate(Vector3::UP 	* MOVE_SPEED * timeStep);
        hologramNode->Translate(Vector3::UP * MOVE_SPEED * timeStep);
    }
    if (input->GetKeyDown(KEY_Q)) {
        // cameraNode_->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
        hologramNode->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
    }

    if (input->GetKeyDown(KEY_P)) {
        vhp->SetSagitalCut(0.2f, 0.5f);
        vhp->SetCoronalCut(0.2f, 0.5f);
        vhp->SetAxialCut(0.08f, 0.5f);
        // vhp->SetModelTransparency(0.05f);
    }
    if (input->GetKeyDown(KEY_O)) {
        // inverse logic for transparency onto VHP model
    }
    // move translate
    if (input->GetKeyDown(KEY_UP)) {
        float magnetudeInRadiusFunction = (30 / polarRadius_);
        hologramNode->Translate(
            Vector3::FORWARD *
            (MOUSE_SENSITIVITY / (6 * magnetudeInRadiusFunction) *
             mouseMove.y_));
        hologramNode->Translate(
            Vector3::RIGHT * (MOUSE_SENSITIVITY /
                              (6 * magnetudeInRadiusFunction) * -mouseMove.x_));
    }
    // zoom
    if (input->GetKeyDown(KEY_DOWN)) {
        polarRadius_ += MOUSE_SENSITIVITY / 6 * mouseMove.y_;
    }

    // slide zoom
    if (input->GetKeyDown(KEY_G)) {
        float radius = slideGrab->GetRadius();
        slideGrab->SetRadius(
            radius + (MOUSE_SENSITIVITY / 6 * (radius / 30) * mouseMove.y_));
    }

    if (input->GetMouseMoveWheel())
        // mouse scroll
        cameraGrab->SumRadius(input->GetMouseMoveWheel() * 0.1);

    if (input->GetKeyPress(KEY_RIGHT)) {
        slideComponent->NextSlide();
    }
    if (input->GetKeyPress(KEY_LEFT)) {
        slideComponent->PreviousSlide();
    }

    isholding = input->GetMouseButtonDown(MOUSEB_RIGHT);

    // updateCameraPosition();

    // normalize polar radius
    if (polarRadius_ > 200) polarRadius_ = 200;
    if (polarRadius_ < 0.3f) polarRadius_ = 0.3f;
}

void MyCustomApplication::updateCameraPosition() {
    // moveNodeArroundOtherNode(yaw_, pitch_, polarRadius_, cameraNode_,
    //                          hologramNode->GetPosition());
}
#endif

void MyCustomApplication::Start() {
    // Execute base class startup
    Sample::CreateScene();  // create fulldome's scene

#ifdef fpmed_allow_cpp_application

    // debug text specific.
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();
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

    level = new CLevelData(context_);

    if (level->InitScene(scene_)) {
        level->InitTVComponentForSceneNode("TV");
    } else {
        engine_->Exit();
    }
    // create C++ app
    CreateScene();

#endif

    Sample::Start();

#ifdef fpmed_allow_scripted_application
    frameworkScriptInstance->Execute("void FpmedStart()");

#endif
}

void MyCustomApplication::Stop() {
    if (level) delete level;

    // Perform optional cleanup after main loop has terminated
}

#ifdef fpmed_allow_cpp_application
void MyCustomApplication::HandleUpdates(StringHash eventType,
                                        VariantMap& eventData) {
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    MoveCamera(timeStep);

    Vector3 posCam, posHolo;

    posHolo = hologramNode->GetPosition();
    posCam = cameraNode_->GetPosition();
    Urho3D::Camera* camComp = cameraNode_->GetComponent<Urho3D::Camera>();
    double angle_Z_axis = triangulateTarget(posHolo.x_, posHolo.y_, posHolo.z_,
                                            posCam.x_, posCam.y_, posCam.z_);
    int sector = angle_Z_axis / 0.9f;

    if (sector <= 398) {
        // hologramPlane->SetMaterial(holoTextArray[sector]);
    }
    if (selected_serv == 1) {
        using namespace std;
        Quaternion q = camComp->GetFaceCameraRotation(
            hologramNode->GetWorldPosition(),
            hologramNode->GetWorldRotation() + Quaternion(0.0f, 90.0f, 0.0f),
            FC_ROTATE_XYZ);
        Vector3 v = q.EulerAngles();
        hologramNode->SetRotation(q);
        hologramNode->Rotate(Quaternion(-90.0f, 0.0f, 0.0f));
        // hologramNode->SetWorldRotation(Quaternion(20.0f, 0.0f, 0.0f));
        char text[512];

        fpmed::Vec2<float> camCoords = cameraGrab->GetCoordinates();

        // make a class for debug text compnent
        sprintf(text,
                "SECTOR: %d\nCAM: %f, %f, %f\nZ AXIS ANGLE: %f\nyaw: %f "
                "pitch: %f\nFface cam: %f, %f, %f \nExt command %s",
                sector, posCam.x_, posCam.y_, posCam.z_, angle_Z_axis,
                camCoords.getX(), camCoords.getY(), v.x_, v.y_, v.z_,
                commandString.c_str());

        String txt = String(text);
        debTex->SetText(txt);
        debTex->SetColor(Color(255, 0, 0));
    }
}

#endif