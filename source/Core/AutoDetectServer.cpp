#include "AutoDetectServer.h"

TNETClient *fndProjClient = 0;
TNETServer *fndProjServer = 0;

std::string tempBuffer;

// Warning: dont forget to call deinitializeAutoDetecServer)_, after using
// this resource
void initializeAutoDetecServer() {
    fndProjServer = new TNETServer(PORT_ASK_FOR_PROJECTORS_LOCAL_NETWORK);
    fndProjClient = new TNETClient();
}
void UpdateAutoDetectProjectorServer() {
    // try listen for incomming messages
    while (fndProjServer->update(8) == TEE_MESSAGE)  // as fast as possible
    {
        // just debbuging the info :)
        printf("[%s]>: %s\n", fndProjServer->getPacket().getSenderIP(),
               fndProjServer->getMessage());
        char *recMess = fndProjServer->getMessage();

        if (strncmp(recMess, "APH?", 4) ==
            0)  // the response should be something like:
                // YAPH-MACHINE_NAME-PORT_TO_LISTEN_TO
        {
            // we will store the retrieved server as: "MACHINE_NAME;REACH_PORT"
            strtok(fndProjServer->getMessage(),
                   "?\0");  // ignore the first part, next strtok should get the
                            // port for answer to

            int portForAnswer = atoi(strtok(NULL, "?\0"));  // port to answer

            char answerMess[512];
            memset(answerMess, 0, 512);

            sprintf(
                answerMess, "YAPH%%%s%%%d", getenv("COMPUTERNAME"),
                calibrationServerPort);  // WARNING SPECIFIC WINDOWS FUNCTION,
                                         // TODO make translation for unix
            fndProjClient->send(answerMess, portForAnswer,
                                fndProjServer->getPacket().getSenderIP(),
                                strlen(answerMess));
        }
    }
}

// this allows to respond for commands such as enable and disable the grid
/*
return meaning

55 - dome grid on
56 - dome grid off

TODO: create enumeration

*/
int UpdateAuxiliarGlobalCommands() {
    // try listen for incomming messages
    while (fndProjServer->update(8) == TEE_MESSAGE)  // as fast as possible
    {
        char *command = 0;

        // just debbuging the info :)
        printf("[%s]>: %s\n", fndProjServer->getPacket().getSenderIP(),
               fndProjServer->getMessage());
        char *recMess = fndProjServer->getMessage();
        command = strtok(recMess, ";");

        if (!strcmp("GRID", command)) {
            char *gridState = strtok(NULL, ";");

            if (!strcmp("ON", gridState)) return 55;
            if (!strcmp("OFF", gridState)) return 56;
        }
    }
    return 0;
}

void sendExternalCommand(std::string command, int ntries) {
    TNETClient clinet = TNETClient(0, 0);
    const char *message = command.c_str();
    for (int i = 0; i < ntries;
         ++i)  // send it at least 5 times to increase the
               // chances of the client receiving our command
        clinet.send((char *)message, PORT_ASK_FOR_PROJECTORS_LOCAL_NETWORK,
                    "255.255.255.255", command.length());
}

// Any command can be sent and interpreted with values
// TODO: create enumeration
// TODO: move it to another class
std::string updateEnetCommands() {
    // try listen for incomming messages
    while (fndProjServer->update(8) == TEE_MESSAGE)  // as fast as possible
    {
        char *command = 0;

        // debbuging the info
        printf("[%s]>: %s\n", fndProjServer->getPacket().getSenderIP(),
               fndProjServer->getMessage());

        char *recMess = fndProjServer->getMessage();
        std::string fullCommand(recMess);
        return fullCommand;
    }
    return std::string("");
}

bool isCommandEquals(std::string command, std::string compareStr) {
    char *cmdparam = strtok((char *)command.c_str(), ";");
    if (std::string(cmdparam) == compareStr) {
        return true;
    }
    return false;
}

float getCommandValueAsFloat(std::string command) {
    const char *cmdChar = command.c_str();

    char *cmdparam = strtok((char *)cmdChar, ";");
    char *result = strtok(NULL, ";");

    return (float)atof(result);
}

void deinitializeAutoDetecServer() {
    delete fndProjServer;
    delete fndProjClient;
}
