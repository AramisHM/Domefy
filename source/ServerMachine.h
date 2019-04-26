/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017 Aramis Hornung Moraes
	See Fpmed.h for license and conditions for using this code
*/

#ifndef SERVERMACHINE_H
#define SERVERMACHINE_H
#include "CoreParameters.h"
#include "FpmedCore.h"
#include "TNet.h"
#include "string.h"
#include "vector2.h"

#include "irrXML.h"
//Irrlicht namespaces
using namespace irr;
using namespace io;

typedef struct ServerProperties // A struct that stores our list of servers that the projector can try communicate to.
{
	int ComPort, SceneReplicationPort; // The system communication port for handshake, and the port used for scene replication by Urho3D
	fpmed::Vec2<int> resolution; // Used to store resolution
	int fullScreen;

}ServerProperties;

class ServerMachine {
private:

	TNETServer *serv;
	TNETServer *beaconServ; // used to any-server discovery for the projectors.
	ServerProperties properties;


public:
	ServerMachine(char *propertiesFile = defaultServerFileName);
	~ServerMachine();
	int Update(); // LISTEN THE NETWORK FOR PROJECTORS
	int UpdateServerBeacon(); // Make server beacon listen for projectors trying to discover any server
	int loadServersProperties(char *file_path);
	void setServerProperties(ServerProperties prop);
	ServerProperties getServerProperties();
	/// Show or hide domegrid
    void ToggleDomeGrid();
};

#endif
