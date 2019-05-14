#include "MyCustomApplication.h"

#include <iostream>
#include <sstream>

// TEST COMPONENT
#include "GrabbableUI.h"

MyCustomApplication* application;

MyCustomApplication::MyCustomApplication(Context* context) : Sample(context) {
// ENABLE SCRIPTS
#ifdef fpmed_allow_scripted_application
    context_->RegisterSubsystem(new Script(context_));
#endif

#ifdef fpmed_allow_cpp_application

    // TODO: optimize this loge with componenets
    slideXDeg = 260.0f;
    slideYDeg = 0.0f;
    slideDistance = 30.0f;
    slideGBUI = 0;

    polarCam = true;
    coordX = 0.0f;
    corodY = 0.0f;
    xaccel = 0;  // used to give inertia effect
    yaccel = 0;
    hologramNode = 0;  // must neutralize it for testing that helps
                       // differentiate server from projector instances
    isholding = false;
    accelDecayIteration = 0;
    currentSlideIndex = 0;
    polarRadius_ = 30.0f;

    // Register an object factory, in this
    // case, the slideanimator
    context_->RegisterFactory<SlideTransitionAnimatior>();
    context_->RegisterFactory<GrabbableUI>();
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

// Move a node arround according to a sphere space system
void moveNodeArroundOtherNode(float yaw, float pitch, float radius,
                              Urho3D::Node* node, Urho3D::Vector3 reference,
                              Urho3D::Vector3 correction = Vector3(0.0f, -90.0f,
                                                                   0.0f)) {
    float radiusMagnetude = Cos(Abs(pitch));
    Vector3 oldpos = reference;
    node->SetPosition(
        Vector3((Cos(yaw) * radius * radiusMagnetude) + oldpos.x_,
                (Sin(pitch) * radius) + oldpos.y_,
                (-Sin(yaw) * radius * radiusMagnetude) + oldpos.z_));

    // Construct new orientation for the camera scene node from yaw and pitch.
    // Roll is fixed to zero
    node->SetRotation(
        Quaternion(pitch + correction.x_, yaw + correction.y_, correction.z_));
}

#ifdef fpmed_allow_cpp_application
// Cpp implementation of a static scene for the application
void MyCustomApplication::CreateScene() {
    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    renderer->SetTextureFilterMode(FILTER_ANISOTROPIC);
    renderer->SetTextureAnisotropy(16);
    renderer->SetTextureQuality(QUALITY_MAX);

    // Zone + lighting + fog
    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    //  directional light
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.5f, -1.0f, 0.5f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(1.0f, 1.0f, 1.0f));
    light->SetSpecularIntensity(5.0f);

    // floor
    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            Node* floorNode = scene_->CreateChild("FloorTile");
            floorNode->SetPosition(Vector3(x * 20.5f, -10.0f, y * 20.5f));
            floorNode->SetScale(Vector3(20.0f, 1.0f, 20.f));
            StaticModel* floorObject =
                floorNode->CreateComponent<StaticModel>();
            floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            floorObject->SetMaterial(
                cache->GetResource<Material>("Materials/BlackGrid.xml"));
        }
    }

    // Some random mesh for testing custom componenets
    {
        Node* componentNodeTest = scene_->CreateChild("TestComponent");
        componentNodeTest->SetPosition(Vector3(0.5f, 4.0f, 0.5f));
        StaticModel* floorObject =
            componentNodeTest->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        floorObject->SetMaterial(
            cache->GetResource<Material>("Materials/vhp/coronal/222.xml"));
    }

    // Hologram node - The actual custom code
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

    int height_offset = 5;

    for (int h = 0; h < N_SLICES; ++h) {
        slicesMaterials[h] = 0;  // null initialization
    }

    // Visible Human Project - VHP
    for (int h = 4450; h < N_SLICES; h = h + 3) {
        Urho3D::Material* mushroomMat = cache->GetResource<Material>(
            "Materials/vhp/" + Urho3D::String(6189 - h) + ".xml");

        if (!mushroomMat) {
            slicesMaterials[h] = 0;
            continue;
        }

        std::stringstream ss;
        ss << "VHP-";
        ss << h;

        ss << "\0";
        std::string nodeName = ss.str();
        printf("\n\n added slice %s\n\n", nodeName.c_str());
        Node* floorNode = scene_->CreateChild(nodeName.c_str());
        floorNode->SetPosition(
            Vector3(0.0f, h * 0.0035f - height_offset, 0.0f));
        floorNode->SetScale(Vector3(10.0f, 0.0f, 5.13f));
        StaticModel* floorObject = floorNode->CreateComponent<StaticModel>();
        floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));

        mushroomMat->SetShaderParameter("MatDiffColor",
                                        Color(1.1f, 1.1f, 1.1f, 1.0f));
        floorObject->SetMaterial(mushroomMat);
        slicesMaterials[h] = mushroomMat;  // STORE IT FOR LATER REFERENCING
    }

    // ------- slide node PROTOTYPE of FDS file -------
    s.LoadSlides("./presentation/set.xml");
    int maxSize = s.getNumberOfSlides();
    int slideTextRefId = 0;

    // iterate the slides
    for (int i = 0; i < maxSize; ++i) {
        slidesMaterialArray[i] =
            cache->GetResource<Material>(s.slides.at(i).materialPath.c_str());

        if (s.slides[i].hasRefPoint == true) {
            Node* nodeSet = scene_->CreateChild("hologramPointOfInterest");
            nodeSet->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

            BillboardSet* billboardObject =
                nodeSet->CreateComponent<BillboardSet>();
            billboardObject->SetNumBillboards(1);
            billboardObject->SetMaterial(
                cache->GetResource<Material>("Materials/point.xml"));
            billboardObject->SetSorted(true);

            Billboard* bb = billboardObject->GetBillboard(0);
            bb->position_ = Vector3(s.slides[i].refPoint.getX(),
                                    s.slides[i].refPoint.getY(),
                                    s.slides[i].refPoint.getZ());
            bb->size_ = Vector2(0.5f, 0.5f);
            bb->enabled_ = true;
            billboardObject->Commit();

            float fontSize = 15.0f;

            // add text to reference point if any
            if (s.slides[i].text != std::string("")) {
                s.slides[i].interestPointIndex = slideTextRefId;

                Node* shapeTextNode = scene_->CreateChild("text1");

                interestPointTexts_[slideTextRefId] =
                    shapeTextNode;  // save reference

                shapeTextNode->SetPosition(Vector3(
                    s.slides[i].refPoint.getX(), s.slides[i].refPoint.getY(),
                    s.slides[i].refPoint.getZ()));

                Text3D* shapeText = shapeTextNode->CreateComponent<Text3D>();
                shapeText->SetFont(
                    cache->GetResource<Urho3D::Font>("Fonts/BlueHighway.sdf"),
                    fontSize);
                shapeText->SetText(s.slides[i].text.c_str());
                // shapeText->SetWordwrap(true);
                shapeText->SetWidth(12.0f);

                shapeTextNode->Rotate(Quaternion(-180.0f, 0.0f, 180.0f));
                shapeTextNode->SetScale(
                    Vector3((fontSize * 0.16f), (fontSize * 0.16f),
                            (fontSize * 0.16f)));  // O = i*0.16;
                shapeText->SetColor(Color());

                shapeText->SetFaceCameraMode(FC_ROTATE_XYZ);

                ++slideTextRefId;
            }
        }
    }

    Node* masterSlideNode;
    Node* modelSlideNode;
    masterSlideNode = cameraNode_->CreateChild("Slide");
    slideNode = masterSlideNode;
    modelSlideNode = masterSlideNode->CreateChild("SlideModel");
    masterSlideNode->SetPosition(Vector3(20, 0, 20));

    slideGBUI = masterSlideNode->CreateComponent<GrabbableUI>();
    slideGBUI->SetOrbitableNode(cameraNode_);
    slideModel = modelSlideNode->CreateComponent<StaticModel>();
    slideModel->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

    // TODO: add these nodes in a specific class/module or at least in a
    // separated function call issue-2 - add anatomic cut viewer
    {
        this->viewerNode = cameraNode_->CreateChild("Viewer");
        // TODO: make it a global node in the class
        Node* viewerModelNode = viewerNode->CreateChild("ViewerModel");

        StaticModel* viewerModel =
            viewerModelNode->CreateComponent<StaticModel>();
        viewerModel->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

        viewerModel->SetMaterial(
            cache->GetResource<Material>("Materials/vhp/coronal/222.xml"));
        viewerModelNode->SetScale(Vector3(
            viewerModel->GetMaterial(0)->GetTexture(TU_DIFFUSE)->GetWidth() /
                340,  // we divide by a big number, because images are genaraly
                      // very big unities
            0,
            viewerModel->GetMaterial(0)->GetTexture(TU_DIFFUSE)->GetHeight() /
                340));
        // TODO: set the material

        viewerGrab = viewerNode->CreateComponent<GrabbableUI>();
        viewerGrab->SetOrbitableNode(cameraNode_);
    }

    // no slides at all
    if (s.getNumberOfSlides()) {
        slideModel->SetMaterial(
            slidesMaterialArray[0]);  // starts in the first slide

        // breaks here
        modelSlideNode->SetScale(Vector3(
            slideModel->GetMaterial(0)->GetTexture(TU_DIFFUSE)->GetWidth() / 45,
            0,
            slideModel->GetMaterial(0)->GetTexture(TU_DIFFUSE)->GetHeight() /
                45));
    }
    // modelSlideNode->SetRotation(Quaternion(-90,180,0));
    modelSlideNode->SetRotation(Quaternion(0, 180, 0));

    // Create our custom Mover component that will move & animate the model
    // during each frame's update
    mainSlideAnimator =
        masterSlideNode->CreateComponent<SlideTransitionAnimatior>();
    mainSlideAnimator->setCameraUpdateCallback(cameraCallback);
    // mainSlideAnimator->SetParameters(0, 400);

    // ------- slide node -------

    // More lights
    const unsigned NUM_LIGHTS = 9;
    cameraNode_->SetPosition(Vector3(40, 40, 40));

    for (unsigned i = 0; i < NUM_LIGHTS; ++i) {
        Node* lightNode = scene_->CreateChild("SpotLight");
        Light* light = lightNode->CreateComponent<Light>();

        float angle = 0.0f;

        Vector3 position((i % 3) * 60.0f - 60.0f, 45.0f,
                         (i / 3) * 60.0f - 60.0f);
        Color color(((i + 1) & 1) * 0.5f + 0.5f,
                    (((i + 1) >> 1) & 1) * 0.5f + 0.5f,
                    (((i + 1) >> 2) & 1) * 0.5f + 0.5f);

        lightNode->SetPosition(position);
        lightNode->SetDirection(Vector3(Sin(angle), -1.5f, Cos(angle)));
        light->SetLightType(LIGHT_SPOT);
        light->SetRange(90.0f);
        light->SetRampTexture(
            cache->GetResource<Texture2D>("Textures/RampExtreme.png"));
        light->SetFov(45.0f);
        light->SetColor(color);
        light->SetSpecularIntensity(1.0f);
        light->SetCastShadows(true);
        light->SetShadowBias(BiasParameters(0.00002f, 0.0f));

        light->SetShadowFadeDistance(100.0f);
        light->SetShadowDistance(125.0f);
        light->SetShadowResolution(0.5f);
        light->SetShadowNearFarRatio(0.01f);
    }

    loTransparency = 1.0f;
    cameraNearClipping = 0.01f;
}
#endif

/* Moves the camera arrount */
#ifdef fpmed_allow_cpp_application
void MyCustomApplication::MoveCamera(float timeStep) {
    UI* ui = GetSubsystem<UI>();
    Input* input = GetSubsystem<Input>();

    input->SetMouseVisible(true);

    // Do not move if the UI has a focused element (the console)
    if (ui->GetFocusElement()) return;

    // Movement speed as world units per second
    const float MOVE_SPEED = 5.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.2f;
    const float POLAR_RADIUS = 50.0f;
    const int MOMENTUM_TRIGGER_VALUE = 1;
    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp
    // the pitch between -90 and 90 degrees Only move the camera when the cursor
    // is hidden

    IntVector2 mouseMove = input->GetMouseMove();

    if (input->GetMouseButtonDown(
            MOUSEB_RIGHT)) {  // reset momentum when mouse interacts
        xaccel = 0;
        yaccel = 0;
        slideGBUI->SetMomentum(Vec2<float>(0, 0));
    }

    // apply momentum, AFTER reset
    if (!input->GetMouseButtonDown(MOUSEB_RIGHT) && isholding == true &&
        (Abs(mouseMove.x_) > MOMENTUM_TRIGGER_VALUE ||
         Abs(mouseMove.y_) > MOMENTUM_TRIGGER_VALUE)) {
        xaccel = mouseMove.x_ / 3;
        yaccel = mouseMove.y_ / 3;
    }

    if (input->GetMouseButtonDown(MOUSEB_RIGHT) || xaccel ||
        yaccel) {  // move either by momentum or mouse iteraction
        if (xaccel || yaccel) {
            mouseMove.x_ = xaccel;
            mouseMove.y_ = yaccel;

            if (accelDecayIteration <= 0) {
                if (xaccel > 0) --xaccel;
                if (yaccel > 0) --yaccel;
                if (xaccel < 0) ++xaccel;
                if (yaccel < 0) ++yaccel;
                accelDecayIteration = accelDecayVar;
            } else
                --accelDecayIteration;
        }
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);
    }

    if (input->GetKeyDown(KEY_F)) {
        Vec2<float> slideCoords = slideGBUI->GetCoordinates();
        Vec2<float> move;

        IntVector2 mouseDelta = input->GetMouseMove();
        slideGBUI->SetMomentum(
            Vec2<float>(mouseDelta.x_ / 25, -mouseDelta.y_ / 25));
        move.setX(MOUSE_SENSITIVITY / 4 * (slideDistance / 30) * mouseDelta.x_);
        move.setY(-MOUSE_SENSITIVITY / 4 * (slideDistance / 30) *
                  mouseDelta.y_);
        move.setY(Clamp(move.getY(), -90.0f, 90.0f));

        slideCoords += move;

        slideGBUI->MoveArroundOrbitableReference(
            slideCoords.getX(), slideCoords.getY(), slideDistance,
            Vector3(0, 0, 0), Vector3(90, -90, 0));
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

    if (input->GetKeyDown(KEY_N)) {
        cameraNearClipping -= 0.1f;
        Urho3D::Camera* camComp = cameraNode_->GetComponent<Urho3D::Camera>();
        camComp->SetNearClip(cameraNearClipping);
        printf("camera clip: %f\n", cameraNearClipping);

        // send command to other projectors
        std::stringstream ss;
        ss << "ZSEC;";
        ss << cameraNearClipping;
        ss << ";";
        sendExternalCommand(ss.str());
    }

    if (input->GetKeyDown(KEY_M)) {
        cameraNearClipping += 0.1f;
        Urho3D::Camera* camComp = cameraNode_->GetComponent<Urho3D::Camera>();
        camComp->SetNearClip(cameraNearClipping);
        printf("camera clip: %f\n", cameraNearClipping);

        // send command to other projectors
        std::stringstream ss;
        ss << "ZSEC;";
        ss << cameraNearClipping;
        ss << ";";
        sendExternalCommand(ss.str());
    }

    if (input->GetKeyDown(KEY_P)) {
        loTransparency += 0.01f;
        for (int h = 0; h < N_SLICES; h = h + 1) {
            Urho3D::Material* mushroomMat = slicesMaterials[h];

            if (!mushroomMat) continue;
            mushroomMat->SetShaderParameter(
                "MatDiffColor", Color(1.1f, 1.1f, 1.1f, loTransparency));
        }
        printf("%f\n", loTransparency);

        // send command to other projectors
        std::stringstream ss;
        ss << "TRANSP;";
        ss << loTransparency;
        ss << ";";
        sendExternalCommand(ss.str());
    }
    if (input->GetKeyDown(KEY_O)) {
        loTransparency -= 0.01f;
        for (int h = 0; h < N_SLICES; h = h + 1) {
            Urho3D::Material* mushroomMat = slicesMaterials[h];

            if (!mushroomMat) continue;
            mushroomMat->SetShaderParameter(
                "MatDiffColor", Color(1.1f, 1.1f, 1.1f, loTransparency));
            printf("setted transparency\n", loTransparency);
        }
        printf("%f\n", loTransparency);

        // send command to other projectors
        std::stringstream ss;
        ss << "TRANSP;";
        ss << loTransparency;
        ss << ";";
        sendExternalCommand(ss.str());
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
        if (slideDistance < 0.01f) slideDistance = 0.01f;

        if (slideDistance > 30.0f) slideDistance = 30.0f;

        slideDistance +=
            MOUSE_SENSITIVITY / 6 * (slideDistance / 30) * mouseMove.y_;
    }

    if (input->GetMouseMoveWheel())
        polarRadius_ += input->GetMouseMoveWheel() * 1;

    if (input->GetKeyPress(KEY_RIGHT)) {  // next slide
        if (!s.getNumberOfSlides())       // no slides at all
            return;
        if (currentSlideIndex < s.getNumberOfSlides() - 1) ++currentSlideIndex;
        slideModel->SetMaterial(slidesMaterialArray[currentSlideIndex]);
        // mainSlideAnimator->SetParameters(s.slides[currentSlideIndex],
        // &pitch_,
        //                                  &yaw_, &polarRadius_, 0, 2000);
    }
    if (input->GetKeyPress(KEY_LEFT)) {  // previous slide
        if (!s.getNumberOfSlides())      // no slides at all
            return;
        if (currentSlideIndex > 0) --currentSlideIndex;
        slideModel->SetMaterial(slidesMaterialArray[currentSlideIndex]);
        // mainSlideAnimator->SetParameters(s.slides[currentSlideIndex],
        // &pitch_,
        //                                  &yaw_, &polarRadius_, 0, 2000);
    }
    if (input->GetKeyPress(KEY_F3)) {
        if (selected_serv > 0)  // Servidor
            machineMaster->ToggleDomeGrid();
    }

    isholding = input->GetMouseButtonDown(MOUSEB_RIGHT);

    updateCameraPosition();

    // normalize polar radius
    if (polarRadius_ > 60) polarRadius_ = 60;
    if (polarRadius_ < 0.3f) polarRadius_ = 0.3f;

    // normalize transparency
    if (loTransparency > 1.0f) loTransparency = 1.0f;
    if (loTransparency < 0.01f) loTransparency = 0.01f;
}

void MyCustomApplication::updateCameraPosition() {
    moveNodeArroundOtherNode(yaw_, pitch_, polarRadius_, cameraNode_,
                             hologramNode->GetPosition());
}
#endif

void MyCustomApplication::Start() {
    // Execute base class startup
    Sample::CreateScene();  // create fulldome's scene

#ifdef fpmed_allow_cpp_application
    if (selected_serv == 1) {
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

            for (int i = 0; i < 399; ++i)  // yes, its 399 textures.
            {
                char filename[512];
                char finalPath[1024];
                sprintf(filename, "SAM_%d.xml", seq);
                sprintf(finalPath, ".\\converted_images\\Materials\\%s",
                        filename);
                holoTextArray[i] = cache->GetResource<Material>(finalPath);

                ++seq;
            }
        }
    }
    SubscribeToEvent(E_POSTRENDERUPDATE,
                     URHO3D_HANDLER(MyCustomApplication, HandleUpdates));
    // create C++ app
    CreateScene();
#endif
    Sample::Start();
}

#ifdef fpmed_allow_cpp_application
void MyCustomApplication::HandleUpdates(StringHash eventType,
                                        VariantMap& eventData) {
    using namespace Update;
    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if (isServer())            // is it server?
        MoveCamera(timeStep);  // only server dictates the position of camera

    // code to change the hologram image
    if (isServer()) {
        Vector3 posCam, posHolo;
        if (selected_serv == 0 && hologramNode == 0) {
            hologramNode = scene_->GetChild("Hologram");
            hologramPlane = hologramNode->GetComponent<StaticModel>();
            hologramMaterial = hologramPlane->GetMaterial();
        }
        posHolo = hologramNode->GetPosition();
        posCam = cameraNode_->GetPosition();
        Urho3D::Camera* camComp = cameraNode_->GetComponent<Urho3D::Camera>();
        double angle_Z_axis =
            triangulateTarget(posHolo.x_, posHolo.y_, posHolo.z_, posCam.x_,
                              posCam.y_, posCam.z_);
        int sector = angle_Z_axis / 0.9f;

        if (sector <= 398) {
            // hologramPlane->SetMaterial(holoTextArray[sector]);
        }
        if (selected_serv == 1) {
            using namespace std;
            Quaternion q = camComp->GetFaceCameraRotation(
                hologramNode->GetWorldPosition(),
                hologramNode->GetWorldRotation() +
                    Quaternion(0.0f, 90.0f, 0.0f),
                FC_ROTATE_XYZ);
            Vector3 v = q.EulerAngles();
            hologramNode->SetRotation(q);
            hologramNode->Rotate(Quaternion(-90.0f, 0.0f, 0.0f));
            // hologramNode->SetWorldRotation(Quaternion(20.0f, 0.0f, 0.0f));
            char text[512];

            sprintf(text,
                    "SECTOR: %d\nCAM: %f, %f, %f\nZ AXIS ANGLE: %f\nyaw: %f "
                    "pitch: %f\nFface cam: %f, %f, %f \n"
                    "slide: %f,  %f",
                    sector, posCam.x_, posCam.y_, posCam.z_, angle_Z_axis, yaw_,
                    pitch_, v.x_, v.y_, v.z_, slideXDeg, slideYDeg);

            String txt = String(text);
            debTex->SetText(txt);
            debTex->SetColor(Color(255, 0, 0));
        }
    }
}

void MyCustomApplication::updateRemoteControls() {
    std::string auxiliarCommand = updateExternalCommands();

    if (auxiliarCommand != "") {
        if (isCommandEquals(auxiliarCommand, "TRANSP")) {
            float val = getCommandValueAsFloat(auxiliarCommand);
            printf("\n\n\ntransparency: %f\n\n\n", val);

            for (int h = 1; h < N_SLICES; ++h) {
                std::stringstream ss;
                ss << "VHP-";
                ss << h;
                Urho3D::Node* slice;
                slice = scene_->GetChild(ss.str().c_str());
                if (!slice) continue;
                Urho3D::Material* mat =
                    slice->GetComponent<StaticModel>()->GetMaterial();

                mat->SetShaderParameter("MatDiffColor",
                                        Color(1.1f, 1.1f, 1.1f, val));
            }
        }

        if (isCommandEquals(auxiliarCommand, "ZSEC")) {
            float val = getCommandValueAsFloat(auxiliarCommand);
            printf("\n\n\zsec: %f\n\n\n", val);

            Urho3D::Node* mainCameraNode;
            mainCameraNode = scene_->GetChild("Camera");
            Urho3D::Node* tempCameraNode;
            tempCameraNode = mainCameraNode->GetChild("CameraFrontNode_");
            // probabily has offset
            Urho3D::Node* tempNodeRef;
            if (!tempCameraNode) {
                tempNodeRef = mainCameraNode->GetChild("CameraOffset");
                tempCameraNode = tempNodeRef->GetChild("CameraFrontNode_");
            } else {
                tempNodeRef = mainCameraNode;
            }
            tempCameraNode = tempNodeRef->GetChild("CameraFrontNode_");
            Urho3D::Camera* camComp = tempCameraNode->GetComponent<Camera>();
            camComp->SetNearClip(val);

            tempCameraNode = tempNodeRef->GetChild("CameraUpNode_");
            camComp = tempCameraNode->GetComponent<Camera>();
            camComp->SetNearClip(val);

            tempCameraNode = tempNodeRef->GetChild("CameraBackNode_");
            camComp = tempCameraNode->GetComponent<Camera>();
            camComp->SetNearClip(val);

            tempCameraNode = tempNodeRef->GetChild("CameraRightNode_");
            camComp = tempCameraNode->GetComponent<Camera>();
            camComp->SetNearClip(val);

            tempCameraNode = tempNodeRef->GetChild("CameraLeftNode_");
            camComp = tempCameraNode->GetComponent<Camera>();
            camComp->SetNearClip(val);
        }
    }
}
#endif