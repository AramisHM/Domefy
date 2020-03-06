/*
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: 21th January 2020
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2020 Aramis Hornung Moraes
 */

#ifdef CEF_INTEGRATION
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/WebBrowser/WebBrowser.h>

// CEF specific

#include "UCefApp.h"
#include "simple_app.h"

// constructor and destructor
WebBrowser::WebBrowser(Urho3D::Context *context)
    : Urho3D::LogicComponent(context) {
    defaultStartUrl =
        std::string("file:///./Data/Textures/assets-march/browser-ready.jpeg");
}
WebBrowser::~WebBrowser() {}

// CreateWebBrowser - Creates a WebBrowser on given scene node
void WebBrowser::CreateWebBrowser(int resX, int resY) {
    Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();

    webBrowserNode_ = node_->CreateChild("WebBrowser");
    webBrowser360Node_ = node_->CreateChild("WebBrowser360");
    webBrowserYoutube360Node_ = node_->CreateChild("WebBrowserYoutube360");
    webBrowserYoutube3603DNode_ = node_->CreateChild("WebBrowserYoutube3603D");

    webBrowserGrabbableUI_ = webBrowserNode_->CreateComponent<GrabbableUI>();

    // this is expected to ve linked to the camera, therefore, camera always is
    // at center.
    webBrowserGrabbableUI_->SetOrbitableReference(Vector3(0.0f, 0.0f, 0.0f));
    webBrowserGrabbableUI_->SetRadius(1.0f);

    // the model node can be adjusted, because the root node is always rotated
    // and moved by the spheric coord system, this solves the problem of
    // upsidedown image
    Node *webBrowserModelNode_ =
        webBrowserNode_->CreateChild("WebBrowserModel");
    webBrowserModel_ = webBrowserModelNode_->CreateComponent<StaticModel>();
    webBrowserModel_->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    webBrowserModelNode_->SetScale(Vector3(4.0f, 0.0f, 2.25f));
    webBrowserModelNode_->SetRotation(Quaternion(0, 180.0f, 0.0f));

    // 360 node
    {
        webBrowser360ModelNode_ =
            webBrowser360Node_->CreateChild("WebBrowser360Model");
        webBrowser360Model_ =
            webBrowser360ModelNode_->CreateComponent<StaticModel>();
        webBrowser360Model_->SetModel(
            cache->GetResource<Model>("Models/360projection-2.mdl"));
        webBrowser360ModelNode_->SetScale(Vector3(-64.0f, 64.0f, 64.0f));
        webBrowser360ModelNode_->SetRotation(Quaternion(0, 90.0f, 0.0f));
        webBrowser360ModelNode_->SetEnabled(false);  // default is invisible
    }

    //  youtube 360 node
    {
        webBrowserYoutube360ModelNode_ =
            webBrowserYoutube360Node_->CreateChild("WebBrowserYoutube360Model");
        webBrowserYoutube360Model_ =
            webBrowserYoutube360ModelNode_->CreateComponent<StaticModel>();
        webBrowserYoutube360Model_->SetModel(
            cache->GetResource<Model>("Models/360-youtube-mesh-inverted.mdl"));
        webBrowserYoutube360ModelNode_->SetScale(Vector3(5.0f, 5.0f, 5.0f));
        webBrowserYoutube360ModelNode_->SetRotation(
            Quaternion(-45.0f, 0.0f, 0.0f));
        webBrowserYoutube360ModelNode_->SetEnabled(
            false);  // default is invisible
    }

    //  youtube 360 3D node
    {
        webBrowserYoutube3603DModelNode_ =
            webBrowserYoutube3603DNode_->CreateChild(
                "WebBrowserYoutube3603DModel");
        webBrowserYoutube3603DModel_ =
            webBrowserYoutube3603DModelNode_->CreateComponent<StaticModel>();
        webBrowserYoutube3603DModel_->SetModel(
            cache->GetResource<Model>("Models/youtube-360-3d.mdl"));
        webBrowserYoutube3603DModelNode_->SetScale(Vector3(1.0f, 1.0f, 1.0f));
        webBrowserYoutube3603DModelNode_->SetRotation(
            Quaternion(-45.0f, 0.0f, .0f));
        webBrowserYoutube3603DModelNode_->SetEnabled(
            false);  // default is invisible
    }

    {
        browser_ = new UBrowserImage(context_);
        UI *ui = GetSubsystem<UI>();
        ui->GetRoot()->AddChild(browser_);
        UCefRenderHandle *uCefRenderHandler_ = new UCefRenderHandle(
            CEFBUF_WIDTH, CEFBUF_HEIGHT, CEFBUF_COMPONENTS);
        browser_->Init(uCefRenderHandler_, CEFBUF_WIDTH, CEFBUF_HEIGHT);

        CefMainArgs main_args(NULL);

        // Specify CEF global settings here.
        CefSettings settings;
        settings.multi_threaded_message_loop = true;
        settings.windowless_rendering_enabled = true;
        CefRefPtr<SimpleHandler> simpHandler =
            new SimpleHandler((CefRenderHandler *)uCefRenderHandler_);
        CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();
        // Specify CEF browser settings here.
        CefBrowserSettings browser_settings;
        std::string url;

        url = command_line->GetSwitchValue("url");
        if (url.empty()) {
            url = defaultStartUrl;
        }

        // Information used when creating the native window.
        CefWindowInfo window_info;

        // LUMAK: change to windowless and browser sync
        window_info.SetAsWindowless(NULL, false);

        CefBrowserHost::CreateBrowser(window_info, simpHandler, url,
                                      browser_settings, NULL);

        ResourceCache *cache = GetSubsystem<ResourceCache>();

        // programmatically create a material
        SharedPtr<Material> m(new Material(context_));
        m->SetTechnique(
            0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        m->SetTexture(TU_DIFFUSE, browser_->GetTexture());
        // m->SetLineAntiAlias(true);
        webBrowserModel_->SetMaterial(m);
        webBrowser360Model_->SetMaterial(m);
        webBrowserYoutube360Model_->SetMaterial(m);
        webBrowserYoutube3603DModel_->SetMaterial(m);
    }
    UIRender = true;
    ToggleUIRender();  // hide it at start
}

UBrowserImage *WebBrowser::GetBrowserImage() { return browser_; }

void WebBrowser::ToggleUIRender() {
    UIRender = !UIRender;
    UI *ui = GetSubsystem<UI>();
    if (UIRender) {  // add the ui element
        ui->GetRoot()->SetOpacity(1.0f);
        ui->GetRoot()->SetEnabled(true);
    } else {  // remove the ui
        ui->GetRoot()->SetOpacity(0.0f);
        ui->GetRoot()->SetEnabled(false);
    }
}

void WebBrowser::LoadURL(std::string url) { browser_->LoadURL(url); }

void WebBrowser::DefaultURL() { this->LoadURL(defaultStartUrl); }

// TODO: implement it for something maybe?
void WebBrowser::Update(float timeStep) {}

// Passes the data to GrabbableUI to apply movement and momentum
void WebBrowser::ApplyMouseMoveLegacy(Vec2<int> d) {}
void WebBrowser::ApplyMouseMove(Urho3D::IntVector2 d) {
    webBrowserGrabbableUI_->ApplyMouseMove(fpmed::Vec2<int>(d.x_, d.y_));
}

void WebBrowser::SetZoom(float zoom) {
    webBrowserGrabbableUI_->SetRadius(zoom);
    ApplyMouseMove(Urho3D::IntVector2(
        0, 0));  // hack to update the rendering TODO: actually fix this!
}

void WebBrowser::AddZoom(float zoom) {
    float currZoom = webBrowserGrabbableUI_->GetRadius();
    webBrowserGrabbableUI_->SetRadius(currZoom + (zoom * 0.001f));
    ApplyMouseMove(Urho3D::IntVector2(
        0, 0));  // hack to update the rendering TODO: actually fix this!
}

GrabbableUI *WebBrowser::GetGrabbableUI() { return webBrowserGrabbableUI_; }

void WebBrowser::SetSphericView(bool isSpheric) {
    webBrowser360ModelNode_->SetEnabled(isSpheric);
}

void WebBrowser::SetCubeView(bool isCubic) {
    webBrowserYoutube360ModelNode_->SetEnabled(isCubic);
}

void WebBrowser::SetCube3DView(bool isCubic) {
    webBrowserYoutube3603DModelNode_->SetEnabled(isCubic);
}

#endif