
//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2019
//

#include <Application/MyCustomApplication.h>
#include <Application/Sample.h>
#include <Core/auxiliarAppFuncs.h>
#include <FPMED.H>
#include <Urho3D.h>
#include <Urho3DAll.h>

#include <ahm/net/net.h>

#include <Core/ProgramConfig.h> // Singleton

namespace Urho3D
{
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
} // namespace Urho3D

extern MyCustomApplication *application;
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