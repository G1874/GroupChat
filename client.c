#include "client.h"

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

void* handle_sending_msg(void* p_arguments)
{
    int sockfd = *((int*)p_arguments);
    char buffer[BUFFLEN];
    fflush(stdin);

    while (1)
    {
        memset(buffer, 0, BUFFLEN);

        fgets(buffer, BUFFLEN, stdin);

        if (write(sockfd, buffer, BUFFLEN) < 0) {
            error("Failed to write");
        }
    }

    return NULL;
}

void handle_receiving_msg(const int* sockfd)
{
    int msg_len;
    char buffer[BUFFLEN];
    fflush(stdout);
    
    while (1)
    {
        memset(buffer, 0, BUFFLEN);

        if ((msg_len = read(*sockfd, buffer, BUFFLEN)) < 0) {
            error("Failed to read");
        }

        printf("%s\n", buffer);
    }
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr = {0}, cli_addr = {0};
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    struct hostent* server;
    pthread_t t;
    int sockfd, port;

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

    pthread_create(&t, NULL, handle_sending_msg, (void*)&sockfd);
    handle_receiving_msg(&sockfd);

    pthread_join(t, NULL);

    close(sockfd);

    return EXIT_SUCCESS;
}