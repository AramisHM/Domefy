#include "ProjectorMachine.h"

#include <string>

ProjectorMachine::ProjectorMachine(int listenPort)
{
	// start the networking engine
	startTNetEngine();

	// assign the port this object will use to listen to server, while in hand-shake mode
	listen_port = listenPort;

	// setup a simple udp server and client
	serv = new TNETServer(listen_port);
	client = new TNETClient(0, 0);
}
ProjectorMachine::~ProjectorMachine()
{

	delete client;
	delete serv;
	//stopTNetEngine();
}
//int Update(); // LISTEN THE NETWORK FOR PROJECTORS, What is it used for? TODO : maybe delete this.

// Check if a server is up and running. Detect if the a server is alive. Returns 1 if server detected or 0 if not.
int ProjectorMachine::CheckServerAlive(int n_times, int server_default_port, char *ip)
{
	int tryes = n_times; // number of trys befor giving up

	while(tryes >= 0)
	{
		char requestMessage[32];

		//compose the request message
		sprintf(requestMessage,"AYA-%d", listen_port);	// message is AYA-<MY_LISTENING_PORT> ; AYA = "Are you alive?",
															// if answer is yes, the port ot talk back is glued to the
															// AYA message.

		client->send(requestMessage, server_default_port, ip, strlen(requestMessage));

		if(serv->update(CHECK_SERVER_ALIVE_TIMEOUT_MILISEC) == TEE_MESSAGE) // Has a message arrived to the network ?
		{
			char *masterMachineReplyMessage = (char*) serv->getMessage();	// extract the message
			if(!strncmp(masterMachineReplyMessage,"YES",strlen("YES")))		// Check if message is from server replying me
			{
				free(masterMachineReplyMessage); // DO IT BEFORE RETURN! ...our lib requires us to free it by hand...
				return 1; // The server replied to us, he is up and running.
			}
			free(masterMachineReplyMessage); // ...our lib requires us to free it by hand...
		}
		--tryes;
	}

	return 0; // nobody answered us. No servers available.
}

// Ask if there is ANY server running in local network
int ProjectorMachine::ScanForServersAlive(int n_times, ServerRef *ref)
{

	int tryes = n_times; // number of trys befor giving up

	while(tryes >= 0 && ref != 0)
	{
		char requestMessage[32];

		//compose the request message
		sprintf(requestMessage,"ASA-%d", listen_port);	// message is ASA-<MY_LISTENING_PORT> ; ASA = "Any server alive?",
															// if answer is yes, the port to talk back is "glued" to the
															// ASA message.

		client->send(requestMessage, defaultScanableServerPort, "255.255.255.255", strlen(requestMessage)); // 255.255.255.255 IS A RESERVERD ADDR FOR BROADCAST ACCORDING TO IPV4 PROTOCOL


		if(serv->update(1000) == TEE_MESSAGE) // Has a message arrived to the network ?
		{
		    // The response should be something like "YES;<comm_port>;<replication_port>;"



			char *masterMachineReplyMessage = (char*) serv->getMessage();	// extract the message

			char *firstArguent = 0, *srepPort = 0, *scomPort = 0;
			firstArguent =          strtok(masterMachineReplyMessage,";"); // main head message, should be "YES"
			scomPort =              strtok(NULL,";"); // hold paralel comm port response in string type
			srepPort =              strtok(NULL,";"); // hold replication port response in string type
            char *senderIp = 0;

			int comPort = -1; // should hold the paralel comm port value, converted from the massage in string to int
            int repPort = -1; // should hold the replication port value, converted from the massage in string to int

			if(!strncmp(firstArguent,"YES",strlen("YES")))
			{

                comPort = atoi(scomPort);
			    repPort = atoi(srepPort);

			    // copy the importat info and initialize the others
			    ref->SceneReplicationPort = repPort;
			    ref->ComPort = comPort;
			    senderIp = serv->getPacket().getSenderIP();
			    memset(ref->ip,0,ipSizeChar);
			    strncpy(ref->ip,senderIp,strlen(senderIp));

				free(masterMachineReplyMessage); // DO IT BEFORE RETURN! ...our lib requires us to free it by hand...

				return 1; // The server replied to us, he is up and running.
			}
			free(masterMachineReplyMessage); // ...our lib requires us to free it by hand...
		}
		--tryes;
	}

	return 0; // nobody answered us. No servers available.
}


// Set the ProjectorMachine object listening port (Used for handshaking from porjector machine with the server of the projection environment)
void ProjectorMachine::setPort(int port)
{
	listen_port = port;
}

// Get the ProjectorMachine object listening port (Used for handshaking from porjector machine with the server of the projection environment)
int ProjectorMachine::getPort()
{
	return listen_port;
}


// --- General functions for servers know by the projector ---
// Add a server to the "list" of possibles servers for the object ProjectorMachine try to reach when trying to connect.
void ProjectorMachine::pushServer(ServerRef serverReference)
{
	servers.push_back(serverReference);
}
// Remove all servers from the list of possible servers to try to connect to when starting up the ProjectorMachine object.
void ProjectorMachine::clearServer()
{
	servers.clear();
}
// Return a struct of the server from a given index
ServerRef ProjectorMachine::getServer(int index)
{
	return servers[index];
}
// The number of servers we have registred to try to communicate
int ProjectorMachine::countServers()
{
	return servers.size();
}

// Load Servers File
// Auxiliar function that fills up the server array list (servers we can try rech to when starting up the ProjectorMachine object).
// This function parses the file, witch is supposed to be a XML like file with specific labels to look up for.
int ProjectorMachine::loadServersList(char *path)
{
	IrrXMLReader* xml = createIrrXMLReader(path);

	// strings for storing the data we want to get out of the file
	std::string modelFile;
	std::string messageText;
	std::string caption;

	if(!xml)  // TODO : test if this condition ever reachs a null value when the file is not found
		return -1;

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				if (!strcmp("peer", xml->getNodeName())) // PROJECTOR SEGMENT
				{
					// copy the ip and port data to the Server structure and then push it to the servers vector array.
					char *xmlIp = 0;
					ServerRef server;

					// Initialize default data
					server.ComPort = -1;
					server.SceneReplicationPort = defaultSceneRepPort;
					memset(server.ip,0,ipSizeChar);
					server.ForceConnection = 0;

					xmlIp=(char*)xml->getAttributeValue("ip");
					strncpy(server.ip,xmlIp,strlen(xmlIp));
					server.ComPort=atoi(xml->getAttributeValue("ComPort"));
					server.SceneReplicationPort=atoi(xml->getAttributeValue("SceneReplicationPort"));

					if(xml->getAttributeValue("ForceConnection"))
						server.ForceConnection=atoi(xml->getAttributeValue("ForceConnection"));

					pushServer(server);

				} // END OF PROJECTOR SEGMENT
			}
			break;
		case EXN_ELEMENT_END:
			if (!strcmp("FpMED", xml->getNodeName()))
			{
				//printf("End of FpMED file reached.\n"); // TODO : log info
				break;
			}
			break;
		}
	}
	// delete the xml parser after usage
	delete xml;
	return 1; // success
}
