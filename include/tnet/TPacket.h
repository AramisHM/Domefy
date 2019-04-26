//				  07/08/2015
// TinyEnet - Minimalistic Networking engine
// Wrapper for ENet Networking Engine
// Copyright (c) 2015 Aramis Hornung Moraes
//
//

#ifndef TPACKET_H
#define TPACKET_H

#include <enet/enet.h>

typedef struct t_packet
{
	// We are wrapping the eNet packet event so...
	ENetEvent event;
	
}t_packet;

class TPacket
{
private:
	t_packet packet;
	char host_ip[17]; // ip string + null termination eg. "000.000.000.000/0" = a string of 17 bytes.
public:
	t_packet getPacket();
	/// Adapted for Enet only.
	void setPacket(t_packet pckt);
	char* getSenderIP();
};

#endif