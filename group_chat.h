#ifndef GROUP_CHAT_H
#define GROUP_CHAT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFLEN 512
#define SERVER_BACKLOG 5
#define MAX_CONNECTIONS 100
#define USERNAME_LENGTH 22
#define REQUEST_LENGTH 5
#define MESSAGE_LENGTH 490
#define MESSAGE "M"
#define REQUEST "R"
#define ACKNOWLEDGEMENT "A"
#define LOAD_REQUEST "load"

void error(const char* err);
void* handle_connection(void* p_clientfd);

struct args_struct {
    int arg1;
    struct sockaddr_in arg2;
    int* arg3;
    int* arg4;
    FILE* arg5;
};

#endif