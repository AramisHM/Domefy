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

// constructor and destructor
Slide::Slide(Urho3D::Context* context) : Urho3D::LogicComponent(context) {}
Slide::~Slide() {}

// CreateSlide - Load data and creates the slide model into the root node
void Slide::CreateSlide(Urho3D::String filePath) {
    // TODO: reade the code below and remove unecessary blocks
    // ------- slide node PROTOTYPE of FDS file -------
    slideReader.LoadSlides(
        filePath.CString());  // default was: ./presentation/set.xml
    int maxSize = slideReader.getNumberOfSlides();
    int slideTextRefId = 0;

    Urho3D::ResourceCache* cache = GetSubsystem<ResourceCache>();

    // iterate the slides
    for (int i = 0; i < maxSize; ++i) {
        slidesMaterialArray[i] = cache->GetResource<Material>(
            slideReader.slides.at(i).materialPath.c_str());

        // TODO:
        // The segment below is the code that adds the points of interest in the
        // *scene*. BUT, *THIS* componenet is suggested to be created in the
        // camera node, not in the scene. Therefore, we should migrate this code
        // and log to its own componenet
        // -----------------------------------------------------------------------
        //
        //
        // if (s.slides[i].hasRefPoint == true) {
        //     Node* nodeSet = node_->CreateChild("hologramPointOfInterest");
        //     nodeSet->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

        //     BillboardSet* billboardObject =
        //         nodeSet->CreateComponent<BillboardSet>();
        //     billboardObject->SetNumBillboards(1);
        //     billboardObject->SetMaterial(
        //         cache->GetResource<Material>("Materials/point.xml"));
        //     billboardObject->SetSorted(true);

        //     Billboard* bb = billboardObject->GetBillboard(0);
        //     bb->position_ = Vector3(s.slides[i].refPoint.getX(),
        //                             s.slides[i].refPoint.getY(),
        //                             s.slides[i].refPoint.getZ());
        //     bb->size_ = Vector2(0.5f, 0.5f);
        //     bb->enabled_ = true;
        //     billboardObject->Commit();

        //     float fontSize = 15.0f;

        //     // add text to reference point if any
        //     if (s.slides[i].text != std::string("")) {
        //         s.slides[i].interestPointIndex = slideTextRefId;

        //         Node* shapeTextNode = node_->CreateChild("text1");

        //         interestPointTexts_[slideTextRefId] =
        //             shapeTextNode;  // save reference

        //         shapeTextNode->SetPosition(Vector3(
        //             s.slides[i].refPoint.getX(), s.slides[i].refPoint.getY(),
        //             s.slides[i].refPoint.getZ()));

        //         Text3D* shapeText = shapeTextNode->CreateComponent<Text3D>();
        //         shapeText->SetFont(
        //             cache->GetResource<Urho3D::Font>("Fonts/BlueHighway.sdf"),
        //             fontSize);
        //         shapeText->SetText(s.slides[i].text.c_str());
        //         // shapeText->SetWordwrap(true);
        //         shapeText->SetWidth(12.0f);

        //         shapeTextNode->Rotate(Quaternion(-180.0f, 0.0f, 180.0f));
        //         shapeTextNode->SetScale(
        //             Vector3((fontSize * 0.16f), (fontSize * 0.16f),
        //                     (fontSize * 0.16f)));  // O = i*0.16;
        //         shapeText->SetColor(Color());

        //         shapeText->SetFaceCameraMode(FC_ROTATE_XYZ);

        //         ++slideTextRefId;
        //     }
        // }
    }

    Node* masterSlideNode;
    Node* modelSlideNode;
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
    if (slideReader.getNumberOfSlides()) {
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
    // mainSlideAnimator =
    //     masterSlideNode->CreateComponent<SlideTransitionAnimatior>();
    // mainSlideAnimator->setCameraUpdateCallback(cameraCallback);
    // mainSlideAnimator->SetParameters(0, 400);

    // ------- slide node -------
}

// TODO: implement it for something maybe?
void Slide::Update(float timeStep) {}
void Slide::NextSlide() {
    if (!slideReader.getNumberOfSlides())  // no slides at all
        return;
    if (currentSlideIndex < slideReader.getNumberOfSlides() - 1)
        ++currentSlideIndex;
    slideModel->SetMaterial(slidesMaterialArray[currentSlideIndex]);
    // mainSlideAnimator->SetParameters(slideReader.slides[currentSlideIndex],
    // &pitch_,
    //                                  &yaw_, &polarRadius_, 0, 2000);
}
void Slide::PreviousSlide() {
    if (!slideReader.getNumberOfSlides())  // no slides at all
        return;
    if (currentSlideIndex > 0) --currentSlideIndex;
    slideModel->SetMaterial(slidesMaterialArray[currentSlideIndex]);
    // mainSlideAnimator->SetParameters(s.slides[currentSlideIndex],
    // &pitch_,
    //                                  &yaw_, &polarRadius_, 0, 2000);
}

// Passes the data to GrabbableUI to apply movement and momentum
void Slide::ApplyMouseMove(Vec2<int> d) { slideGrabbableUI->ApplyMouseMove(d); }