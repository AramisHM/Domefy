/*
 * Filename: /home/aramis/Desktop/UDPC/src/net/net.h
 * Path: /home/aramis/Desktop/UDPC/src/net
 * Created Date: Friday, June 7th 2019, 4:15:25 pm
 * Author: Aramis Hornung Moraes
 *
 * Copyright (c) 2019 Aramis Hornung Moraes
 */

#ifndef AHMNET_H
#define AHMNET_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __unix__
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#define MAX_UDP_BUFSIZE 32768 /* About as much as a UDP can handle (64k) */
#define AHMNET_ADDRSTRLEN 22  /* replaces INET_ADDRSTRLEN */

/* TODO: make this a parameter when we create the socket */

// connection struct
typedef struct conn {
    unsigned char ip[128]; /*TODO: how many bytes does a IPV6 needs? */
    unsigned int port;
    char lastip[AHMNET_ADDRSTRLEN]; /* ip from last device to transmit packet */
    unsigned char is_binded;        /* indicates if server or client */

    int sockfd;                  /* socket */
    int raddlen;                 /* byte size of remote's address */
    struct sockaddr_in localadd; /* this process address */
    struct sockaddr_in remotadd; /* remote process address */
    char buf[MAX_UDP_BUFSIZE];   /* message buf */
    int optval;                  /* flag value for setsockopt */
    long int lrps;               /* last received packet size */
} conn;

/* udp_listen - builds up a udp server
 */
int udp_listen(char *ip_port, conn *a);

/* udp_dial - connects to a udp server
 */
int udp_dial(char *ip_port, conn *a);

/* sender_ip - gethostbyaddr: determine who sent the datagram */
char *sender_ip(conn *a);

/* udp_read - listens as a udp server in a given timeout of microseconds. */
int sock_read(conn *a, unsigned int timeout);

/* initializes the engine */
int ahmnet_init();

/* delete engine */
void ahmnet_clean();
/* sock_write - send data on the socket */
int sock_write(conn *a);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AHMNET_H */
