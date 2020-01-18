
//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2019
//

#include <Application/MyCustomApplication.h>
#include "CharacterDemo.h"
#include <Application/Sample.h>
#include <Core/auxiliarAppFuncs.h>
#include <FPMED.H>
#include <Urho3D.h>
#include <Urho3DAll.h>

#include <ahm/net/net.h>

#include <Core/ProgramConfig.h> // Singleton

#ifdef WIN32
#include "windows.h"
#include <ShellAPI.h> // for CommandLineToArgvW
#include <vector>
#include <codecvt>
#include <locale>
#include <string>
#endif

namespace Urho3D
{
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
} // namespace Urho3D

conn extChanel;
std::string commandString;
std::string scriptPath;

void ListenForExternalCommands()
{
    if (sock_read(&extChanel, 1) > 0)
    {
        // printf(
        //     "\nReceived a command via AHMNet from %s, with the following data : %s\n\n",
        //     sender_ip(&extChanel), extChanel.buf);
        commandString = std::string(extChanel.buf);
    }
    else
    {
        commandString = "";
    }
}

#if defined(_WIN32) // NON-CONSOLE LIKE/WINDOWS WAY OF RUNNING PROGRAMS
int RunApplication()
{
    Urho3D::SharedPtr<Urho3D::Context> context(new Urho3D::Context());
    
    
    Urho3D::SharedPtr<MyCustomApplication> application(new MyCustomApplication(context));
    // Urho3D::SharedPtr<CharacterDemo> application(new CharacterDemo(context));
    
    // return application->Run();



    ProgramConfig *p1 = ProgramConfig::GetInstance();
    ahmnet_init();
    udp_listen("127.0.0.1:42871", &extChanel);

    char *argc[] = {"C:\\git\\DomefyCef\\bin\fpmed.exe\0"};

    fpmedInit(1,argc);
    //p1->LoadConfigFile("C:\\git\\DomefyCef\\bin\\config.json\0");

    scriptPath = std::string("C:\\git\\DomefyCef\\bin\\Data\\Scripts\\18_CharacterDemo2.as\0");

    if (application->isApplication())
    {
        application->Prepare();
        application->Start();

        while (application->isApplication())
        {
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    Urho3D::ParseArguments(GetCommandLineW());
    // int exitCode;
    // __try
    // {
    //     exitCode = RunApplication();
    // }
    // __except(Urho3D::WriteMiniDump("Urho3D", GetExceptionInformation()))
    // {
    // }
    return RunApplication();
}
#else // GOOD 'OL INT MAIN()
extern MyCustomApplication *application;
int main(int argc, char *argv[])
{
    ProgramConfig *p1 = ProgramConfig::GetInstance();

    ahmnet_init();
    udp_listen("127.0.0.1:42871", &extChanel);
    fpmedInit(argc, argv);
    p1->LoadConfigFile("./config.json");

    Urho3D::ParseArguments(argc, argv);
    Urho3D::Context *context = new Urho3D::Context();
    application = new MyCustomApplication(context);

    if (argc > 1)
    {
        scriptPath = std::string(argv[1]);
    }
    else
    {
        scriptPath = std::string("Scripts/18_CharacterDemo2.as");
    }

    if (application->isApplication())
    {
        application->Prepare();
        application->Start();

        while (application->isApplication())
        {
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
#endif