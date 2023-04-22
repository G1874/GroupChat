#include "server.h"

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

void* handle_connection(void* p_clientfd) {

}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    int sockfd, newsockfd;
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

    listen(sockfd, SERVER_BACKLOG);

    while (1)
    {
        printf("Waiting for connenction\n");

        if ((newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len)) < 0) {
            error("Failed to accept connection");
        }

        printf("Waiting for data...\n");

        memset(buffer, 0, sizeof(buffer));
        fflush(stdout);

        if ((msg_len = read(newsockfd, buffer, BUFFLEN)) < 0) {
            error("Failed to read");
        } else if (msg_len == 0) {
            break;
        }

        printf("Recived packet from %s:%d\n", inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
        printf("Data: %s", buffer);

        close(newsockfd);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}