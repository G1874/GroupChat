#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFLEN 512
#define N_CLIENTS 5

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    int sockfd, newsockfd[N_CLIENTS];
    int port, msg_len, n_nsock = 0;
    char buffer[BUFFLEN];
    
    memset((char*)&serv_addr, 0, serv_len);
    memset((char*)&cli_addr, 0, cli_len);

    if (argc != 2) {
        error("No port provided");
    }
    
    port = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Failed to create socket");
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, serv_len) < 0) {
        error("Failed to bind socket");
    }

    printf("Waiting for connenction\n");

    listen(sockfd, 5);
    if (n_nsock <= N_CLIENTS) {

        if ((newsockfd[n_nsock] = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len)) < 0) {
            error("Failed to accept connection");
        }

        n_nsock++;
    }

    while (1)
    {
        printf("Waiting for data...\n");

        memset(buffer, 0, sizeof(buffer));
        fflush(stdout);

        // for(int i; i < n_nsock; i++)
        // {
        if ((msg_len = read(newsockfd[0], buffer, BUFFLEN)) < 0) {
            error("Failed to read");
        } else if (msg_len == 0) {
            break;
        }

        printf("Recived packet from %s:%d\n", inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
        printf("Data: %s", buffer);
        // }
    }

    close(sockfd);
    for(int i; i < n_nsock; i++) {
        close(newsockfd[n_nsock]);
    }

    return EXIT_SUCCESS;
}