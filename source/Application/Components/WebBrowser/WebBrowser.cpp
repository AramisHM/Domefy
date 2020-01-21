/*
 * File: WebBrowser.cpp
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Tuesday, 21st May 2019 4:36:52 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Tuesday, 21st May 2019 4:37:15 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2020 Aramis Hornung Moraes
 */
#ifdef CEF_INTEGRATION
#include <Application/Components/GrabbableUI/GrabbableUI.h>
#include <Application/Components/WebBrowser/WebBrowser.h>

// CEF specific
#include "UBrowserImage.h"
#include "UCefApp.h"
#include "simple_app.h"

// constructor and destructor
WebBrowser::WebBrowser(Urho3D::Context *context)
    : Urho3D::LogicComponent(context) {}
WebBrowser::~WebBrowser() {}

// CreateWebBrowser - Load data and creates the WebBrowser model into the root
// node
void WebBrowser::CreateWebBrowser() {
    Urho3D::ResourceCache *cache = GetSubsystem<ResourceCache>();

    webBrowserNode_ = node_->CreateChild("WebBrowser");
    webBrowserGrabbableUI_ = webBrowserNode_->CreateComponent<GrabbableUI>();

    // this is expected to ve linked to the camera, therefore, camera always is
    // at center.
    webBrowserGrabbableUI_->SetOrbitableReference(Vector3(0.0f, 0.0f, 0.0f));
    webBrowserGrabbableUI_->SetRadius(5.0f);
    webBrowserModel_ = webBrowserNode_->CreateComponent<StaticModel>();
    webBrowserModel_->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    webBrowserNode_->SetScale(Vector3(4.0f, 0.0f, 2.25f));

    // modelSlideNode->SetRotation(Quaternion(-90,180,0));
    webBrowserNode_->SetRotation(Quaternion(0, 0.0f, 0));

    {
        UBrowserImage *uBrowserImage_ = new UBrowserImage(context_);
        UCefRenderHandle *uCefRenderHandler_ = new UCefRenderHandle(
            CEFBUF_WIDTH, CEFBUF_HEIGHT, CEFBUF_COMPONENTS);
        uBrowserImage_->Init(uCefRenderHandler_, 800, 600);
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

        // Check if a "--url=" value was provided via the command-line. If
        // so, use that instead of the default URL.
        url = command_line->GetSwitchValue("url");
        if (url.empty()) {
            // url = "http://www.google.com";
            // url = "https://www.youtube.com/watch?v=-fmCoUjOMXU";
            url = "file:///./Data/fpmed/domefy_logo_fullsize.png";
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
        m->SetTechnique(0, cache->GetResource<Technique>(
                               "Techniques/DiffAlphaTranslucent.xml"));
        m->SetTexture(TU_DIFFUSE, uBrowserImage_->GetTexture());
        webBrowserModel_->SetMaterial(m);
    }
}

// TODO: implement it for something maybe?
void WebBrowser::Update(float timeStep) {}

// Passes the data to GrabbableUI to apply movement and momentum
void WebBrowser::ApplyMouseMoveLegacy(Vec2<int> d) {}
void WebBrowser::ApplyMouseMove(Urho3D::IntVector2 d) {
    webBrowserGrabbableUI_->ApplyMouseMove(fpmed::Vec2<int>(d.x_, d.y_));
}

void WebBrowser::SetZoom(float zoom) {}
#endif