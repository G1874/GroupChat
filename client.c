#include "client.h"

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    struct hostent* server;
    int sockfd, port;
    char buffer[BUFFLEN];

    memset(&serv_addr, 0, serv_len);
    memset(&cli_addr, 0, cli_len);

    if (argc != 3) {
        error("No port provided");
    }

    port = atoi(argv[2]);
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("No such host");
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Failed to create socket");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    bcopy((char*)server->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, serv_len)) {
        error("Failed to connect to server");
    }
        
    fflush(stdin);

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        fgets(buffer, sizeof(buffer), stdin);

        if (write(sockfd, &buffer, sizeof(buffer)) < 0) {
            error("Failed to write to socket");
        }
    }

    close(sockfd);

    return EXIT_SUCCESS;
}