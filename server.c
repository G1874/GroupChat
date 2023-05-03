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
    int* n_nsock = (int*)arguments.arg3;
    int* clientfd_list = (int*)arguments.arg4;

    int msg_len;
    char buffer[BUFFLEN];
    
    fflush(stdout);

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        if ((msg_len = read(clientfd, buffer, BUFFLEN)) < 0) {
            break;
        } else if (msg_len == 0) {
            break;
        }
        
        printf("%d - %s\n", ntohs(cli_addr.sin_port), buffer);
        printf("%d\n\n", *n_nsock);

        printf("%d\n", *n_nsock);
        for (int i = 0; i < *n_nsock; i++) {
            if (clientfd_list[i] != 0) {
                if (write(clientfd_list[i], buffer, BUFFLEN) < 0) {
                    printf("Failed to write to %d\n", clientfd_list[i]);
                }
            }
            printf("%d\n", clientfd_list[i]);
        }
    }

    // TODO implement a mutex
    int j = 0;
    for (int i = 0; i < *n_nsock; i++) {
        clientfd_list[j] = ((int*)arguments.arg4)[i];
        j++;
        if (((int*)arguments.arg4)[i] == clientfd) {
            j--;
        }
    }
    *n_nsock--;
    // TODO implement a mutex

    arguments.arg4 = realloc(clientfd_list, *n_nsock);
    free(clientfd_list);

    close(clientfd);

    return NULL;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    struct args_struct arguments;
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    int sockfd, clientfd;
    int* clientfd_list = calloc(1, sizeof(int));
    int port = 8888, n_nsock = 0, client_index = 0;

    memset((void*)&serv_addr, 0, serv_len);
    memset((void*)&cli_addr, 0, cli_len);

    // if (argc != 2) {
    //     error("No port provided");
    // }
    
    // port = atoi(argv[1]);

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
        if (n_nsock < MAX_CONNECTIONS) {
            if ((clientfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len)) < 0) {
                error("Failed to accept connection");
            }

            if ((sizeof(clientfd_list)/sizeof(int)) < (n_nsock + 1)) {
                clientfd_list = realloc(clientfd_list, sizeof(clientfd_list) + sizeof(int));
            }
            clientfd_list[n_nsock] = clientfd;

            n_nsock++;
        }

        arguments.arg1 = (void*)&clientfd;
        arguments.arg2 = (void*)&cli_addr;
        arguments.arg3 = (void*)&n_nsock;
        arguments.arg4 = (void*)clientfd_list;

        pthread_t t;
        pthread_create(&t, NULL, handle_connection, (void*)&arguments);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}