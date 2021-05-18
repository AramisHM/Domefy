/*
 * File: Slide.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Tuesday, 21st May 2019 4:36:52 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Tuesday, 21st May 2019 4:37:15 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/Slide/Slide.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <streambuf>

using json = nlohmann::json;

// constructor and destructor
Slide::Slide(Urho3D::Context *context) : Urho3D::LogicComponent(context) {
    currentSlideIndex = 0;
    nLoadedSlides = 0;
    useMirrorSlide = false;
}
Slide::~Slide() {}

// CreateSlide - Load data and creates the slide model into the root node
void Slide::CreateSlide(Urho3D::String filePath) {
    // TODO: reade the code below and remove unecessary blocks
    // ------- slide node PROTOTYPE of FDS file -------
    // slideReader.LoadSlides(
    //     filePath.CS otring());  // default was: ./presentation/set.xml
    std::string pathString = std::string(filePath.CString());
    nLoadedSlides = this->LoadSlideFromJSON(pathString);

    Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();

    Node *masterSlideNode;
    Node *modelSlideNode;

    Node *mirrorMasterSlideNode;
    Node *mirrorModelSlideNode;

    // Create main slide
    {
        masterSlideNode = node_->CreateChild("Slide");
        slideNode = masterSlideNode;
        modelSlideNode = masterSlideNode->CreateChild("SlideModel");
        masterSlideNode->SetPosition(Vector3(20, 0, 20));

        slideGrabbableUI = masterSlideNode->CreateComponent<GrabbableUI>();
        // slideGrabbableUI->SetOrbitableReference(node_);
        // this is supposed to link to the camera, therefore, camera always is
        // at center.
        slideGrabbableUI->SetOrbitableReference(Vector3(0.0f, 0.0f, 0.0f));
        slideModel = modelSlideNode->CreateComponent<StaticModel>();
        slideModel->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
        float h, w, ps;
        h = 1;
        w = (float)slideTextureArray[0]->GetWidth() /
            (float)slideTextureArray[0]->GetHeight();
        ps = 0.5f;  // proportion size
        slideNode->SetScale(Vector3(w * ps, 0, h * ps));
    }
    // Create mirror slide
    {
        mirrorMasterSlideNode = node_->CreateChild("MirrorSlide");
        mirrorSlideNode = mirrorMasterSlideNode;
        mirrorModelSlideNode =
            mirrorMasterSlideNode->CreateChild("MirrorSlideModel");
        mirrorMasterSlideNode->SetPosition(Vector3(20, 0, 20));

        mirrorSlideGrabbableUI =
            mirrorMasterSlideNode->CreateComponent<GrabbableUI>();
        // slideGrabbableUI->SetOrbitableReference(node_);
        // this is supposed to link to the camera, therefore, camera always is
        // at center.
        mirrorSlideGrabbableUI->SetOrbitableReference(
            Vector3(0.0f, 0.0f, 0.0f));
        mirrorSlideModel = mirrorModelSlideNode->CreateComponent<StaticModel>();
        mirrorSlideModel->SetModel(
            cache->GetResource<Model>("Models/Plane.mdl"));
        float h, w, ps;
        h = 1;
        w = (float)slideTextureArray[0]->GetWidth() /
            (float)slideTextureArray[0]->GetHeight();
        ps = 0.5f;  // proportion size
        mirrorSlideNode->SetScale(Vector3(w * ps, 0, h * ps));
    }

    // no slides at all
    if (nLoadedSlides) {
        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        m->SetTexture(TU_DIFFUSE, slideTextureArray[0]);

        slideModel->SetMaterial(m);  // starts in the first slide
        modelSlideNode->SetScale(Vector3(800 / 45, 0, 600 / 45));

        mirrorSlideModel->SetMaterial(m);  // starts in the first slide
        mirrorModelSlideNode->SetScale(Vector3(800 / 45, 0, 600 / 45));
    }
    // modelSlideNode->SetRotation(Quaternion(-90,180,0));
    modelSlideNode->SetRotation(Quaternion(0, 180, 0));
    mirrorModelSlideNode->SetRotation(Quaternion(0, 180, 0));

    // mirror starts hiden
    useMirrorSlide = false;
    mirrorSlideModel->SetEnabled(false);
}

// TODO: implement it for something maybe?
void Slide::Update(float timeStep) {}
void Slide::NextSlide() {
    printf("loaded slides %d", nLoadedSlides);
    if (!nLoadedSlides)  // no slides at all
        return;
    if (currentSlideIndex < nLoadedSlides - 1) ++currentSlideIndex;

    // set size
    float h, w, ps;
    h = 1;
    w = (float)slideTextureArray[currentSlideIndex]->GetWidth() /
        (float)slideTextureArray[currentSlideIndex]->GetHeight();
    ps = 0.5f;  // proportion size

    slideNode->SetScale(Vector3(w * ps, 0, h * ps));
    slideModel->GetMaterial(0)->SetTexture(
        TU_DIFFUSE, slideTextureArray[currentSlideIndex]);

    // FIXME: TODO: code is repeating... make a function for setting the
    // material and scale
    if (useMirrorSlide) {
        mirrorSlideNode->SetScale(Vector3(w * ps, 0, h * ps));
        mirrorSlideModel->GetMaterial(0)->SetTexture(
            TU_DIFFUSE, slideTextureArray[currentSlideIndex]);
    }
}
void Slide::PreviousSlide() {
    printf("loaded slides %d", nLoadedSlides);
    if (!nLoadedSlides)  // no slides at all
        return;
    if (currentSlideIndex > 0) --currentSlideIndex;

    // set size
    float h, w, ps;
    h = 1;
    w = (float)slideTextureArray[currentSlideIndex]->GetWidth() /
        (float)slideTextureArray[currentSlideIndex]->GetHeight();
    ps = 0.5f;  // proportion size

    slideNode->SetScale(Vector3(w * ps, 0, h * ps));
    slideModel->GetMaterial(0)->SetTexture(
        TU_DIFFUSE, slideTextureArray[currentSlideIndex]);

    if (useMirrorSlide) {
        mirrorSlideNode->SetScale(Vector3(w * ps, 0, h * ps));
        mirrorSlideModel->GetMaterial(0)->SetTexture(
            TU_DIFFUSE, slideTextureArray[currentSlideIndex]);
    }
}

// Passes the data to GrabbableUI to apply movement and momentum
void Slide::ApplyMouseMoveLegacy(Vec2<int> d) {
    slideGrabbableUI->ApplyMouseMove(d);
}

void Slide::SetCoordinates(Urho3D::IntVector2 d) {
    slideGrabbableUI->SetCoordinates(Vec2<float>(d.x_, d.y_));
}

void Slide::ApplyMouseMove(Urho3D::IntVector2 d) {
    slideGrabbableUI->ApplyMouseMove(fpmed::Vec2<int>(d.x_, d.y_));
}

void Slide::SetZoom(float zoom) {
    slideGrabbableUI->SetRadius(zoom);
    ApplyMouseMove(Urho3D::IntVector2(
        0, 0));  // hack to update the rendering TODO: actually fix this!
}

void Slide::MirrorApplyMouseMove(Urho3D::IntVector2 d) {
    if (useMirrorSlide) {
        mirrorSlideGrabbableUI->ApplyMouseMove(fpmed::Vec2<int>(d.x_, d.y_));
    }
}
void Slide::MirrorSetCoordinates(Urho3D::IntVector2 d) {
    if (useMirrorSlide) {
        mirrorSlideGrabbableUI->SetCoordinates(Vec2<float>(d.x_, d.y_));
    }
}
void Slide::MirrorSetZoom(float zoom) {
    if (useMirrorSlide) {
        mirrorSlideGrabbableUI->SetRadius(zoom);
        MirrorApplyMouseMove(Urho3D::IntVector2(
            0, 0));  // hack to update the rendering TODO: actually fix this!
    }
}

// LoadSlide
int Slide::LoadSlideFromJSON(std::string path) {
    for (int i = 0; i < MAX_SLIDE_COUNT; ++i) {
        slideTextureArray[i] = 0;
    }

    std::ifstream filest(path);
    json slideData;
    filest >> slideData;

    // Get projection viewport configurations
    std::string fileType = slideData["type"];
    std::string fileName = slideData["name"];

    int nSlidesLoaded = 0;
    // If any preset file, load it
    if (slideData["slides"].size() > 0) {
        Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();

        for (const auto &item : slideData["slides"]) {
            std::string imageName = item["image"].get<std::string>();
            printf("%s", imageName.c_str());

            // create material

            Urho3D::Texture2D *t = cache->GetResource<Urho3D::Texture2D>(
                Urho3D::String(imageName.c_str()));
            if (!t) continue;

            t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
            t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
            t->SetNumLevels(1);

            // set texture format
            t->SetMipsToSkip(QUALITY_HIGH, 0);
            t->SetFilterMode(FILTER_TRILINEAR);
            // t->SetSize(CEFBUF_WIDTH, CEFBUF_HEIGHT,
            // Graphics::GetRGBAFormat());

            // set modes

            // save slide data
            slideTextureArray[nSlidesLoaded] = t;
            ++nSlidesLoaded;
        }
    } else {
        printf("Couldn't load slides\n\n");
    }
    // fflush(stdin);
    // getchar();
    return nSlidesLoaded;
}

void Slide::ToggleMirrorSlide() {
    useMirrorSlide = !useMirrorSlide;
    mirrorSlideModel->SetEnabled(useMirrorSlide);
}