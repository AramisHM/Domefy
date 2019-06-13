
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
#include <fstream>

#include <streambuf>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Urho3D {
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
}  // namespace Urho3D

extern MyCustomApplication *application;
conn extChanel;
std::string commandString;

void testJSON() {
    // instead, you could also write (which looks very similar to the JSON
    // above)
    json j2 = {{"pi", 3.141},
               {"happy", true},
               {"name", "Niels"},
               {"nothing", nullptr},
               {"answer", {{"everything", 42}}},
               {"list", {1, 0, 2}},
               {"object", {{"currency", "USD"}, {"value", 42.99}}}};

    printf("%s", j2.dump().c_str());

    std::ifstream t("./pucpr-arena-digital.json");
    json myConfig;
    t >> myConfig;

    for (const auto &item : myConfig["presets"]) {
        printf("%s", item["name"].get<std::string>().c_str());
    }
}

void ListenForExternalCommands() {
    if (sock_read(&extChanel, 1) > 0) {
        printf(
            "\nReceived a command via AHMNet from %s, with the following data: "
            "%s\n\n",
            sender_ip(&extChanel), extChanel.buf);
        commandString = std::string(extChanel.buf);
    } else {
        commandString = "";
    }
}

int main(int argc, char *argv[]) {
    ahmnet_init();
    udp_listen("127.0.0.1:42871", &extChanel);
    fpmedInit(argc, argv);

    testJSON();

    Urho3D::ParseArguments(argc, argv);
    Urho3D::Context *context = new Urho3D::Context();
    application = new MyCustomApplication(context);
    Urho3D::Input *input = application->GetSubsystem<Urho3D::Input>();

    if (serverPropertiesFileDirectory[0] == 0)
        machineMaster = new ServerMachine(defaultServerFileName);
    else
        machineMaster = new ServerMachine(serverPropertiesFileDirectory);

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