/*
 * PROPRIETARY SOFTWARE, You must get consent from owners to use it!
 * Copyright (c) 2018, 2019, 2020 Pteronura.com - www.pteronura.com
 */

#ifndef NET_H
#define NET_H
#define nal 22
#define maxudp 30000
#ifdef __cplusplus
extern "C" {
#endif
#ifdef WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
typedef struct cn {
    struct sockaddr_in rad;
    unsigned char ip[128];
    int ov;
    unsigned int port;
    char lip[nal];
    int sockfd;
    int raddlen;
    char buf[maxudp];
    struct sockaddr_in lad;
    long int lps;
} cn;
// server
int nudsrv(char *ipp, cn *a);
// ip from
char *nfip(cn *a);
// initialize engine
int ninudp();
// stop engine
void nstpudp();
// write socket
int nwrudp(cn *a);
// read socket
int nrdudp(cn *a, unsigned int to);
// client
int nudcli(char *ipp, cn *a);

#ifdef __cplusplus
}
#endif
#endif