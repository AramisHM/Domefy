/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017
   Aramis Hornung Moraes See Fpmed.h for license and conditions for using this
   code
*/

#include "ServerMachine.h"

// Irrlicht namespaces
using namespace irr;
using namespace io;

ServerMachine::ServerMachine(char *propertiesFile) {
    // start the engine
    startTNetEngine();

    // create a new properties val and fill it with default data
    properties.ComPort = defaultComPort;
    properties.SceneReplicationPort = defaultSceneRepPort;
    fpmed::Vec2<int> res(800, 600);
    properties.resolution = res;
    properties.fullScreen = 0;

    loadServersProperties(propertiesFile);

    // setup a simple udp server
    serv = new TNETServer(properties.ComPort);
    beaconServ = new TNETServer(defaultScanableServerPort);
}
ServerMachine::~ServerMachine() {
    //	stopTNetEngine();
    delete serv;
    delete beaconServ;
}
// int Update(); // LISTEN THE NETWORK FOR PROJECTORS :: TODO remove it later?

// Make server listen for projectors
int ServerMachine::Update() {
    char *message;
    char *token;
    if (serv->update(1) == TEE_MESSAGE)  // a message arrived to the network
    {
        message = (char *)serv->getMessage();  // extract the message
        token = strtok(message, " -;\t\n");
        do {
            if (!strcmp(token, "AYA")) {
                token = strtok(NULL, " -;\t\n");
                int projector_listen_port = 0;
                projector_listen_port = atoi(token);
                if (projector_listen_port >=
                    1)  // if we have the projector's port, answer it.
                {
                    char ackMessage[4] = {'Y', 'E', 'S', '\0'};
                    TNETClient clinet = TNETClient(0, 0);
                    clinet.send(ackMessage, projector_listen_port,
                                serv->getPacket().getSenderIP(), 3);
                }
            }
            token = strtok(NULL, " -;\t\n");
        } while (token != NULL);

        free(message);
        token = 0;
    }
    return 0;
}

// Make server beacon listen for projectors trying to discover any server
int ServerMachine::UpdateServerBeacon()  // TODO : make test to check the
                                         // stability of all those strncpy 'n
                                         // stuff
{
    char *message;
    char *token;
    if (beaconServ->update(10) ==
        TEE_MESSAGE)  // a message arrived to the network
    {
        message = (char *)beaconServ->getMessage();  // extract the message
        token = strtok(message, " -;\t\n");
        do {
            if (!strcmp(token, "ASA"))  // any server alive?
            {
                token = strtok(NULL, " -;\t\n");
                int projector_listen_port = 0;
                projector_listen_port = atoi(token);
                if (projector_listen_port >=
                    1)  // if we have the projector's port, answer it.
                {
                    char ackMessage[16] = {'Y',  'E',  'S',  ';',  '\0', '\0',
                                           '\0', '\0', '\0', '\0', '\0', '\0',
                                           '\0', '\0', '\0', '\0'};
                    char ipv4StringCommPort[6] = {'\0', '\0', '\0',
                                                  '\0', '\0', '\0'};
                    char ipv4StringReplPort[6] = {'\0', '\0', '\0',
                                                  '\0', '\0', '\0'};

                    sprintf(ipv4StringCommPort, "%d", properties.ComPort);
                    sprintf(ipv4StringReplPort, "%d",
                            properties.SceneReplicationPort);

                    strncpy(ackMessage + 4, ipv4StringCommPort,
                            strlen(ipv4StringCommPort));
                    strncpy(ackMessage + 4 + strlen(ipv4StringCommPort), ";",
                            1);
                    strncpy(ackMessage + 4 + strlen(ipv4StringCommPort) + 1,
                            ipv4StringReplPort, strlen(ipv4StringReplPort));
                    strncpy(ackMessage + 4 + strlen(ipv4StringCommPort) + 1 +
                                strlen(ipv4StringReplPort),
                            ";", 1);

                    TNETClient clinet = TNETClient(0, 0);

                    clinet.send(ackMessage, projector_listen_port,
                                beaconServ->getPacket().getSenderIP(), 3);
                }
            }
            token = strtok(NULL, " -;\t\n");
        } while (token != NULL);

        free(message);
        token = 0;
    }
    return 0;
}

// Carregar arquivo de servidores
int ServerMachine::loadServersProperties(char *file_path) {
    IrrXMLReader *xml = createIrrXMLReader(file_path);

    if (!xml) return -1;
    while (xml && xml->read()) {
        switch (xml->getNodeType()) {
            case EXN_ELEMENT: {
                if (!strcmp("server", xml->getNodeName())) {
                    properties.ComPort =
                        atoi(xml->getAttributeValue("ComPort"));
                    properties.SceneReplicationPort =
                        atoi(xml->getAttributeValue("SceneReplicationPort"));
                }
                if (!strcmp("vector2d", xml->getNodeName()) &&
                    !strcmp("ServerScreenResolution",
                            xml->getAttributeValue("name"))) {
                    fpmed::Vec2<int> vector;
                    vector.atovec2((char *)xml->getAttributeValue("value"));
                    properties.resolution = vector;
                }
                if (!strcmp("ServerFullScreen", xml->getNodeName())) {
                    properties.fullScreen =
                        atoi(xml->getAttributeValue("value"));
                }
            } break;
            case EXN_ELEMENT_END:
                if (!strcmp("FpMED", xml->getNodeName())) {
                    // printf("End of FpMED file reached4.\n");
                    break;
                }
                break;
        }
    }
    // delete the xml parser after usage
    delete xml;
    return 1;
}

void ServerMachine::setServerProperties(ServerProperties prop) {
    properties = prop;
}
ServerProperties ServerMachine::getServerProperties() { return properties; }

// enables of disables the projectors dome grid.
void ServerMachine::ToggleDomeGrid() {
    TNETClient clinet = TNETClient(0, 0);

    // message to toggle grid on or off
    char message[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    dome_grid = !dome_grid;

    if (dome_grid)
        sprintf(message, "GRID;ON;", 9);
    else
        sprintf(message, "GRID;OFF;", 9);
    for (int i = 0; i < 5; ++i)  // send it at least 5 times to increase the
                                 // chances of the client receiving our command
        clinet.send(message, PORT_ASK_FOR_PROJECTORS_LOCAL_NETWORK,
                    "255.255.255.255",
                    9);  // SEND COMMAND TO CALIBRATOR CHANNEL
}
