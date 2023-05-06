#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFLEN 512
#define SERVER_BACKLOG 5
#define MAX_CONNECTIONS 100

void error(const char* err);
void* handle_connection(void* p_clientfd);

struct args_struct {
    int arg1;
    struct sockaddr_in arg2;
    int arg3;
    int* arg4;
    FILE* arg5;
};

#endif