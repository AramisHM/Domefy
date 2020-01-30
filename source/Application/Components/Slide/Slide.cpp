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
}
Slide::~Slide() {}

// CreateSlide - Load data and creates the slide model into the root node
void Slide::CreateSlide(Urho3D::String filePath) {
    // TODO: reade the code below and remove unecessary blocks
    // ------- slide node PROTOTYPE of FDS file -------
    // slideReader.LoadSlides(
    //     filePath.CString());  // default was: ./presentation/set.xml
    std::string s = std::string(filePath.CString());
    nLoadedSlides = this->LoadSlideFromJSON(
        "D:\\GoPath\\src\\github."
        "com\\AramisHM\\Domefy\\bin\\slides\\aramis\\aramis.json");

    Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();

    Node *masterSlideNode;
    Node *modelSlideNode;
    masterSlideNode = node_->CreateChild("Slide");
    slideNode = masterSlideNode;
    modelSlideNode = masterSlideNode->CreateChild("SlideModel");
    masterSlideNode->SetPosition(Vector3(20, 0, 20));

    slideGrabbableUI = masterSlideNode->CreateComponent<GrabbableUI>();
    // slideGrabbableUI->SetOrbitableReference(node_);
    // this is supposed to link to the camera, therefore, camera always is at
    // center.
    slideGrabbableUI->SetOrbitableReference(Vector3(0.0f, 0.0f, 0.0f));
    slideModel = modelSlideNode->CreateComponent<StaticModel>();
    slideModel->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

    // no slides at all
    if (nLoadedSlides) {
        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(0, cache->GetResource<Technique>(
                               "Techniques/DiffAlphaTranslucent.xml"));
        m->SetTexture(TU_DIFFUSE, slideTextureArray[0]);
        slideModel->SetMaterial(m);  // starts in the first slide

        // breaks here
        modelSlideNode->SetScale(Vector3(800 / 45, 0, 600 / 45));
    }
    // modelSlideNode->SetRotation(Quaternion(-90,180,0));
    modelSlideNode->SetRotation(Quaternion(0, 180, 0));
}

// TODO: implement it for something maybe?
void Slide::Update(float timeStep) {}
void Slide::NextSlide() {
    printf("loaded slides %d", nLoadedSlides);
    if (!nLoadedSlides)  // no slides at all
        return;
    if (currentSlideIndex < nLoadedSlides - 1) ++currentSlideIndex;
    slideModel->GetMaterial(0)->SetTexture(
        TU_DIFFUSE, slideTextureArray[currentSlideIndex]);
}
void Slide::PreviousSlide() {
    printf("loaded slides %d", nLoadedSlides);
    if (!nLoadedSlides)  // no slides at all
        return;
    if (currentSlideIndex > 0) --currentSlideIndex;
    slideModel->GetMaterial(0)->SetTexture(
        TU_DIFFUSE, slideTextureArray[currentSlideIndex]);
}

// Passes the data to GrabbableUI to apply movement and momentum
void Slide::ApplyMouseMoveLegacy(Vec2<int> d) {
    slideGrabbableUI->ApplyMouseMove(d);
}
void Slide::ApplyMouseMove(Urho3D::IntVector2 d) {
    slideGrabbableUI->ApplyMouseMove(fpmed::Vec2<int>(d.x_, d.y_));
}

void Slide::SetZoom(float zoom) {
    slideGrabbableUI->SetRadius(zoom);
    ApplyMouseMove(Urho3D::IntVector2(
        0, 0));  // hack to update the rendering TODO: actually fix this!
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
#ifdef FPMED_LATEST_URHO3D
            t->SetFilterMode(FILTER_NEAREST_ANISOTROPIC);
#else
            t->SetFilterMode(FILTER_NEAREST);
#endif
            t->SetAddressMode(COORD_U, ADDRESS_CLAMP);
            t->SetAddressMode(COORD_V, ADDRESS_CLAMP);
            t->SetNumLevels(1);

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