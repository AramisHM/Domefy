//				  07/08/2015
// TinyEnet - Minimalistic Networking engine
// Wrapper for ENet Networking Engine
// Copyright (c) 2015 Aramis Hornung Moraes
//
//

#ifndef TNET_H
#define TNET_H

#include <enet/enet.h>
#include <stdio.h>

#include "TServer.h"
#include "TClient.h"
#include "TPacket.h"

	#ifdef _WIN32 // Note the underscore: without it, it's not MSDN official!
	// Windows (x64 and x86)
	#include <winsock.h>

	//wsock32.lib
	//#pragma comment(lib, "WSOCK32.lib")
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "winmm.lib")
	#pragma comment(lib, "TNet.lib")
	#endif

enum {
	TEE_MESSAGE = 1,
	TEE_CONNECTION = 2,
	TEE_DISCONNECTION = 3
};

// Always call this before doing anithing with the enigne.
int startTNetEngine();

#endif