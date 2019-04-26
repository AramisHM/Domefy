//				  07/08/2015
// TinyEnet - Minimalistic Networking engine
// Wrapper for ENet Networking Engine
// Copyright (c) 2015 Aramis Hornung Moraes
//
//

#ifndef TClient_H
#define TClient_H

#include <enet/enet.h>
#include <stdio.h>
#include "TPacket.h"

class TNETClient
{
private:
	ENetAddress address;
    ENetHost *client;
    ENetPeer *peer;
    char message[1024];
    ENetEvent event;
    int eventStatus;
public:
	TNETClient(int port = 12340, char * server_ip = "localhost");
	~TNETClient();
	int update(int uSecs);
	int send_peer(char* message, int len = 0);
	int send(char* message, int port = 12340, char *ip = "localhost", int len = 0);
};

int send(char* message, int port, char *ip, int len);
#endif