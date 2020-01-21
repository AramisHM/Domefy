#ifndef CEFWEBBROWSER_H
#define CEFWEBBROWSER_H
#include <Core/CoreParameters.h>
#include <string>

#include <Urho3DAll.h>  // urho3d types

#include <fstream>
#include <streambuf>

#include "UCefApp.h"
#include "simple_app.h"

// Singleton class that holds all configurations for our software.

class CEFWebBrowser {
   public:
    // This is how clients can access the single instance
    static CEFWebBrowser *GetInstance();
    void Init(Urho3D::Context *context_);
    void ShutDown();

   protected:
    SharedPtr<UCefApp> uCefApp_;

   private:
    static CEFWebBrowser *_instance;
    CEFWebBrowser();
    ~CEFWebBrowser();
    CEFWebBrowser(const CEFWebBrowser &);
    CEFWebBrowser &operator=(const CEFWebBrowser &);
};

#endif