#pragma once
#ifndef CREATE_CONNECT
#define CREATE_CONNECT

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

typedef struct sockaddr_in SAIN;
typedef struct sockaddr SA;

int startUp(u_short *port);
int clientStartup(int *sock);

#endif // !CREATE_CONNECT
