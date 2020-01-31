
//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2020
//

#include <Application/MyCustomApplication.h>
#include <Application/Sample.h>
#include <Core/auxiliarAppFuncs.h>
#include <FPMED.H>
#include <Urho3D.h>
#include <Urho3DAll.h>

#ifdef CEF_INTEGRATION
#include <Application/WebBrowser/CEFWebBrowser.h>
#endif

#include <Core/ProgramConfig.h>  // Singleton
#include <ahm/net/net.h>

#ifdef WIN32
#ifdef _MSC_VER
#include <atlstr.h>
#include <shellapi.h>  // for CommandLineToArgvW
#endif
#include <codecvt>
#include <cstring>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include "windows.h"
#endif

namespace Urho3D {
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
}  // namespace Urho3D

// extern MyCustomApplication *application;
conn extChanel;
std::string commandString;
std::string scriptPath;
#ifdef CEF_INTEGRATION
std::string defaultCefUrl =
    "file:///./Data/Textures/assets-march/pucpr-shadown.png";
#endif

void ListenForExternalCommands() {
    if (sock_read(&extChanel, 1) > 0) {
        // printf(
        //     "\nReceived a command via AHMNet from %s, with the following data
        //     : %s\n\n", sender_ip(&extChanel), extChanel.buf);
        commandString = std::string(extChanel.buf);
    } else {
        commandString = "";
    }
}

int main(int argc, char *argv[]) {
    Urho3D::ParseArguments(argc, argv);
    Urho3D::Context *context = new Urho3D::Context();
    MyCustomApplication *application = new MyCustomApplication(context);
    ProgramConfig *p1 = ProgramConfig::GetInstance();

    ahmnet_init();

    udp_listen("127.0.0.1:42871", &extChanel);
    fpmedInit(argc, argv);
    p1->LoadConfigFile("./config.json");

    if (argc > 1 && argc < 3) {
        scriptPath = std::string(argv[1]);
    } else {
        scriptPath = std::string("./Data/Scripts/DefaultPresentation.as");
    }

    if (argc >= 3) {  // url for browser
        defaultCefUrl = std::string(argv[2]);
    }

    if (application->isApplication()) {
        application->Prepare();
        application->Start();

        while (application->isApplication()) {
            ListenForExternalCommands();
            application->RunFrameC();
        }
        application->Stop();
    }

    application->Stop();
    delete application;
    delete context;
    ahmnet_clean();

    return 0;
}
