
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
#include <libLameNet.h>          // network

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

char *extChanel;  // udp server
std::string commandString;
std::string scriptPath;
std::string viewportConfigPath = "";
#ifdef CEF_INTEGRATION
std::string defaultCefUrl =
    "file:///./Data/Textures/assets-march/browser-ready.jpeg";
#endif

void ListenForExternalCommands() {
  char *recdata;
  recdata = LameServRead(extChanel);
  if (strcmp(recdata, "") != 0) {
    commandString = std::string(recdata);
    printf("data: %s", recdata);
  } else {
    commandString = "";
  }
}

int main(int argc, char *argv[]) {
  Urho3D::ParseArguments(argc, argv);
  Urho3D::Context *context = new Urho3D::Context();
  MyCustomApplication *application = new MyCustomApplication(context);
  ProgramConfig *p1 = ProgramConfig::GetInstance();

  LameNetStart();

#ifdef CEF_INTEGRATION
  extChanel = LameListen("42872");  // udp server
#else
  extChanel = LameListen("42871");
#endif
  fpmedInit(argc, argv);

#ifndef CEF_INTEGRATION
  if (argc <= 3) {  // config
    viewportConfigPath = std::string(std::string(argv[2]));
  }
#endif

  p1->LoadConfigFile("./config.json", viewportConfigPath);

  if (argc > 1 && argc < 4) {
    scriptPath = std::string(argv[1]);
  } else {
    scriptPath = std::string("./Data/Scripts/DefaultPresentation.as");
  }

#ifdef CEF_INTEGRATION
  if (argc >= 4) {  // url for browser
    defaultCefUrl = std::string(argv[3]);
  }
#endif
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
  LameNetStop();

  return 0;
}
