
/*	This file is part of the FpMED (Anydome) Project Copyright (c) 2014-2016
   Aramis Hornung Moraes See FpmedCore.h for license and conditions for using
   this code
*/
#ifndef PROJECTORMACHINE_H
#define PROJECTORMACHINE_H

#include <Core/CoreParameters.h>  // Our general purpouse definitions are declared here.

#include <vector>  // C++'s standart vector calss for the projectors array, nothing to do with the vec2,3,4 that are graphic vectors
#include "string.h"     // C++'s standart string class
#include "tnet/TNet.h"  // Includes our particular Networking Lib, that is actually a wrapper for the ENet Lib.

// Irrlicht Stuff - We use irrlicht XML module for parsering our files.
#include "irrXML.h"
using namespace irr;
using namespace io;

typedef struct ServerRef  // A struct that stores our list of servers that the
                          // projector can try communicate to.
{
    char ip[ipSizeChar];  // the environment's server ip address.
    int ComPort;  // The port used to open socket for initial handshake between
                  // the projector_machines/client_or_players and the server.
                  // The technology used for socket communication is the ENet
                  // Wrapper: TNet.
    int SceneReplicationPort;  // The port used to open socket for scene
                               // replication. The technology used for
                               // communication is Urho's Networking Module.
    int ForceConnection;       // TODO
} ServerRef;

class ProjectorMachine {
   private:
    TNETClient *client;  // SEND REQUESTS TO THE MASTER MACHINE
    TNETServer *serv;    // LISTEN FOR RESPONSE FROM THE MASTER MACHINE
    std::vector<ServerRef>
        servers;      // Array of servers that we can try to "talk" to
    int listen_port;  // port that the projector (us, a.k.a. ProjectorMachine)
                      // listens to

   public:
    ProjectorMachine(int port);
    ~ProjectorMachine();
    // Check if a server is up and running. Detect if the a server is alive.
    // Returns 1 if server detected or 0 if not.
    int CheckServerAlive(int n_times = 5,
                         int server_default_port = defaultComPort,
                         char *ip = "localhost");

    // Ask if there is ANY server running in local network
    int ScanForServersAlive(int n_times, ServerRef *ref);

    // Set the ProjectorMachine object listening port (Used for handshaking from
    // porjector machine with the server of the projection environment)
    void setPort(int port);

    // Get the ProjectorMachine object listening port (Used for handshaking from
    // porjector machine with the server of the projection environment)
    int getPort();

    // --- General functions for environment ---
    // Add a server to the "list" of possibles servers for the object
    // ProjectorMachine try to reach when trying to connect.
    void pushServer(ServerRef serverReference);

    // Remove all servers from the list of possible servers to try to connect to
    // when starting up the ProjectorMachine object.
    void clearServer();

    // Return a struct of the server from a given index
    ServerRef getServer(int index);

    // The number of servers we have registred to try to communicate
    int countServers();

    // Load Servers File
    // Auxiliar function that fills up the server array list (servers we can try
    // reach to when starting up the ProjectorMachine object). This function
    // parses the file, witch is supposed to be a XML like file with specific
    // labels to look up for.
    int loadServersList(char *path);
};

#endif
