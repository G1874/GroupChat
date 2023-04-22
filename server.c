#include "server.h"

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

void* handle_connection(void* p_arguments) {

    struct args_struct arguments = *((struct args_struct*)p_arguments);

    int clientfd = *((int*)arguments.arg1);
    struct sockaddr_in cli_addr = *((struct sockaddr_in*)arguments.arg2);

    // free(p_arguments);

    int msg_len;
    char buffer[BUFFLEN];
    
    fflush(stdout);

    while(1)
    {
        // printf("Waiting for data...\n");

        memset(buffer, 0, sizeof(buffer));

        if ((msg_len = read(clientfd, buffer, BUFFLEN)) < 0) {
            error("Failed to read");
        } else if (msg_len == 0) {
            break;
        }

        // printf("%s:%d - %s\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
        printf("%d - %s\n", ntohs(cli_addr.sin_port), buffer);
    }

    close(clientfd);

    return NULL;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    struct args_struct arguments;
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    int sockfd, newsockfd;
    int port, n_nsock = 0;

    memset((char*)&serv_addr, 0, serv_len);
    memset((char*)&cli_addr, 0, cli_len);

    arguments.arg1 = malloc(sizeof(int));
    arguments.arg2 = malloc(sizeof(struct sockaddr_in));

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
        // printf("Waiting for connenction\n");

        if ((newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len)) < 0) {
            error("Failed to accept connection");
        }

        arguments.arg1 = (void*)&newsockfd;
        arguments.arg2 = (void*)&cli_addr;

        // struct args_struct* p_arguments = malloc(sizeof(struct args_struct));
        // p_arguments = &arguments;

        pthread_t t;
        pthread_create(&t, NULL, handle_connection, (void*)&arguments);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}