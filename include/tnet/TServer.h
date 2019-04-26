//				  07/08/2015
// TinyEnet - Minimalistic Networking engine
// Wrapper for ENet Networking Engine
// Copyright (c) 2015 Aramis Hornung Moraes
//
//

#ifndef TSERVER_H
#define TSERVER_H

#include <enet/enet.h>
#include <stdio.h>
#include "TPacket.h"

// maximum number of messages that can be stacked
#define TSTACK_MAX 256

typedef struct Stack {
    void    *data[TSTACK_MAX];
    int     size;

}Stack;

class TNETServer
{
private:
	Stack message_stack;
	ENetAddress address;
	ENetHost *server;
    	ENetEvent enet_event; // enet hack
    	int eventStatus;
public:
	TNETServer(int port);
	~TNETServer();
	int update(int uSecs);
	// WARWNING! DO NOT modify this variable directly, dont forget to
	// make a copy of this variable, this is a direct reference to the
	// message data packat.
	char* getMessage();
	TPacket getPacket();
	void setPacket(TPacket pckt);
	int send(char* message, int len = 0);
	int stackPushMessage(char *msg);
	void *stackPopMessage();
	int stackSize();
	void *stackHead();
};
#endif
