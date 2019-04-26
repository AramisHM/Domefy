//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2019
//

#include "FPMED.H"
#include "MyCustomApplication.h"
#include "Sample.h"
#include "Urho3D.h"
#include "Urho3DAll.h"
#include "auxiliarAppFuncs.h"

#include "ScriptEditor.h"

namespace Urho3D {
class Button;
class Connection;
class Scene;
class Text;
class UIElement;
}  // namespace Urho3D

extern ServerMachine *machineMaster;
extern ProjectorMachine *projectorMachine;
extern TNETServer *ScriptServ;
extern MyCustomApplication *application;

int main(int argc, char *argv[]) {
    ScriptEditor sEdit;

    // Used to ensure a one time loop at the next while block
    int oneTimeLoop = 0;

    fpmedInit(argc, argv);

    // If we are not editing script, we will do a one time loop only
    if (!flag_edit_script) oneTimeLoop = 1;

    while (flag_edit_script || oneTimeLoop) {
        // We entered once, next time it will exit
        oneTimeLoop = 0;

        sEdit.editScriptLoop();

        // All parameters have been loaded,
        // now lets start Urho3D.
        Urho3D::ParseArguments(argc, argv);
        Urho3D::Context *context = new Urho3D::Context();
        application = new MyCustomApplication(context);
        Urho3D::Input *input = application->GetSubsystem<Urho3D::Input>();

        // It is server, create main server parallel responder
        if (isServer()) {
            if (serverPropertiesFileDirectory[0] == 0)
                machineMaster = new ServerMachine(defaultServerFileName);
            else
                machineMaster =
                    new ServerMachine(serverPropertiesFileDirectory);
        } else {  // its projector, create calibration server responder
            projectorMachine = new ProjectorMachine((Random(1000) + 8000));
            calibrationServerPort =
                (Random(1000) +
                 7000);  // define a random port for calibration server
                         // (used to adjust the projectors view)
            printf("\n\nCALIBRATION PORT IS: %d\n\n", calibrationServerPort);
            projectorMachine->loadServersList(serverPropertiesFileDirectory);
            initializeAutoDetecServer();  // FEATURE USED TO ANSWER FOR
                                          // AUTODETECTION MESSAGE PROTOCOL,
                                          // HELPS OTHER PROGRAMS DISCOVER THIS
                                          // PROJECTPOR INSTANCE
        }

        // tells us if we are showing the stand-by scene
        bool standByScene = false;

        if (isProjector()) {  // projector
            application->Prepare();
            application->Sample::CreateScene();
            application->Sample::Start();
            input->SetMouseVisible(true);
        }
        while (application->isApplication()) {
            if (isProjector()) {
                if (!standByScene) {  // prepare projector stand-by scene
                    application->Sample::CreateScene();
                    application->SetupViewport();
                    application->SubscribeToEvents();
                    application->createStandbyScene();
                    standByScene = true;
                }
                ServerRef serverAdd;
                strncpy(serverAdd.ip, "127.0.0.1\0", 10);
                serverAdd.ComPort = defaultComPort;
                serverAdd.SceneReplicationPort = defaultSceneRepPort;
                serverAdd.ForceConnection = 0;
                application->RunFrameC();
                application->loopCalibrateCamera();
                UpdateAutoDetectProjectorServer();

                if (find_any_local_server ==
                    1) {  // try locate any server within local network?
                    printf(
                        "Scanning local network for servers... (BROADCAST)\n");
                    if (projectorMachine->ScanForServersAlive(2, &serverAdd)) {
                        printf("Found one server! Attempting connection...\n");
                    }
                } else {
                    // Test for local server, then test the list of servers,
                    // finally we connect to it.
                    printf("Trying local server @%s:%d replication_port: %d\n",
                           serverAdd.ip, serverAdd.ComPort,
                           serverAdd.SceneReplicationPort);

                    if (!projectorMachine->CheckServerAlive(
                            1, serverAdd.ComPort,
                            serverAdd.ip)) {  // if local server dont work try
                                              // the external
                        int serverListSize = projectorMachine->countServers();
                        if (serverListSize) {
                            for (int s = 0; s < serverListSize; ++s) {
                                serverAdd = projectorMachine->getServer(
                                    s);  // replace the default information with
                                         // loaded file
                                printf(
                                    "Trying server @%s:%d replication_port: "
                                    "%d\n",
                                    serverAdd.ip, serverAdd.ComPort,
                                    serverAdd.SceneReplicationPort);
                                if (projectorMachine->CheckServerAlive(
                                        1, serverAdd.ComPort, serverAdd.ip) ||
                                    serverAdd.ForceConnection) {
                                    printf(
                                        "Server @%s:%d replication_port: %d is responding... \
										   Trying to establish final connection.\n\n",
                                        serverAdd.ip, serverAdd.ComPort,
                                        serverAdd.SceneReplicationPort);
                                    break;
                                }
                            }
                        }
                    }
                }

                // If force connection is true, we will simply try to replicate
                // the scene with informed scene rep port.
                if (projectorMachine->CheckServerAlive(2, serverAdd.ComPort,
                                                       serverAdd.ip) ||
                    serverAdd.ForceConnection) {
                    printf(
                        "Connection established, server @%s:%d "
                        "replication_port: %d\n\n\n",
                        serverAdd.ip, serverAdd.ComPort,
                        serverAdd.SceneReplicationPort);
                    // application->Prepare();
                    application->ConnectToSceneServer(
                        serverAdd.SceneReplicationPort, serverAdd.ip);
                    // standByScene = false; // we are going to switch to remote
                    // server scene

                    while (application->isApplication() &&
                           application->IsConnectedToServer()) {
                        application->RunFrameC();
                        standByScene = false;

                        application
                            ->updateRemoteControls();  // model transparency and
                                                       // zsec
                        // se chegar algum comando para calibrar a imagem
                        // application->loopCalibrateCamera();
                        // TODO: conflito, o calibrador consome os pacotes da
                        // funcao de comandos externos

                        // keep checking if server is running
                        UpdateAutoDetectProjectorServer();
                    }
                    application->Stop();
                } else {
                    printf(
                        "No connection with server @%s:%d replication_port: "
                        "%d\n",
                        serverAdd.ip, serverAdd.ComPort,
                        serverAdd.SceneReplicationPort);
                }
            } else {  // server
                application->Prepare();
                application->Start();
                application->StartSceneServer(
                    machineMaster->getServerProperties().SceneReplicationPort);

                while (application->isApplication()) {  // roda a aplicacao Urho
                                                        // COMO Servidor
                    application->RunFrameC();
                    machineMaster->Update();  // respond to projectors trying to
                                              // connect to me directly
                    machineMaster
                        ->UpdateServerBeacon();  // respond for projector
                                                 // looking for any server in
                                                 // the room.
                }
                application->Stop();
                // application->CloseApplication();
            }
        }

        application->Stop();

        delete application;
        delete context;
        delete ScriptServ;
        ScriptServ = 0;  // used to receive commands to script module

        if (isServer())
            delete machineMaster;
        else {  // projector
            delete projectorMachine;
            deinitializeAutoDetecServer();
        }
    }
    return 0;
}