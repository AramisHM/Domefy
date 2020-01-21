#include <Application/WebBrowser/CEFWebBrowser.h>

CEFWebBrowser* CEFWebBrowser::_instance = NULL;

// constructor
CEFWebBrowser::CEFWebBrowser() {
    uCefApp_ = 0;
    // CefExecuteProcess() needs to be call in the constructor, otherwise,
    // you'll get multiple windows when using SDL
    CefMainArgs main_args(NULL);

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line
    // and, if this is a sub-process, executes the appropriate logic.
    int exit_code = CefExecuteProcess(main_args, NULL, NULL);
}
void CEFWebBrowser::ShutDown() {
    if (uCefApp_) {
        uCefApp_->DestroyAppBrowser();
        uCefApp_ = NULL;
    }

    // calling CefShutdown() w/o having called CefInitialize() once
    // causes exception due to no context created for cef
    Time::Sleep(100);
    CefShutdown();
}
CEFWebBrowser::~CEFWebBrowser() { this->ShutDown(); }

CEFWebBrowser* CEFWebBrowser::GetInstance() {
    if (_instance == NULL) {
        _instance = new CEFWebBrowser();
    }
    return (_instance);
}

void CEFWebBrowser::Init(Urho3D::Context* context_) {
    uCefApp_ = new UCefApp(context_);
    uCefApp_->CreateAppBrowser();
}
