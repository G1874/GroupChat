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
    void* arg1;
    void* arg2;
    void* arg3;
    void* arg4;
};

#endif