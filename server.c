#include "group_chat.h"

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

void* handle_connection(void* p_arguments) 
{
    struct args_struct arguments = *((struct args_struct*)p_arguments);

    int clientfd = arguments.arg1;
    struct sockaddr_in cli_addr = arguments.arg2;
    int* n_nsock = arguments.arg3;
    int* clientfd_list = arguments.arg4;
    FILE* p_file = arguments.arg5;

    int msg_len;
    char buffer[BUFFLEN];
    char msg_buffer[MESSAGE_LENGTH];
    char client_username[USERNAME_LENGTH];
    char request[REQUEST_LENGTH];

    fflush(stdout);

    if (read(clientfd, client_username, USERNAME_LENGTH) >= 0) {
        while (1)
        {
            memset(buffer, 0, BUFFLEN);

            if ((msg_len = read(clientfd, buffer, BUFFLEN)) <= 0) {
                break;
            }

            printf("%d - %s", ntohs(cli_addr.sin_port), buffer);

            if (buffer[0] == 'M') {
                for (int i = 0; i < (msg_len-1); i++) {
                    msg_buffer[i] = buffer[i+1];
                }

                memset(buffer, 0, BUFFLEN);
                strcat(buffer, client_username);
                strcat(buffer, msg_buffer);

                p_file = fopen("file.csv", "a+");
                fprintf(p_file,"%s, %s\n", client_username, msg_buffer);
                fclose(p_file);

                for (int i = 0; i <= *n_nsock; i++) {
                    if (clientfd_list[i] != 0 && clientfd_list[i] != clientfd) {
                        if (write(clientfd_list[i], buffer, BUFFLEN) < 0) {
                            printf("Failed to write to %d\n", clientfd_list[i]);
                        }
                    }
                    printf("%d ", clientfd_list[i]);
                }
                printf("%c", '\n');
            } else if (buffer[0] == 'R') {
                memset(request, 0, REQUEST_LENGTH);

                for(int i = 0; i < REQUEST_LENGTH; i++) {
                    request[i] = buffer[i+1];
                }
                
                if (strncmp(request, LOAD_REQUEST, sizeof(LOAD_REQUEST))) {

                }
            }
        }
    }

    // TODO implement a semaphor
    for (int i = 0; i <= *n_nsock; i++) {
        if (clientfd_list[i] == clientfd) {
            clientfd_list[i] = 0;
        }
    }
    // TODO implement a semaphor

    close(clientfd);

    return NULL;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr = {0}, cli_addr = {0};
    struct args_struct arguments;
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    int sockfd, clientfd;
    int clientfd_list[MAX_CONNECTIONS] = {0};
    int port, n_nsock = 0;

    FILE* p_file;
    p_file = fopen("file.csv", "a+");
    fprintf(p_file,"User Name, Message\n");
    fclose(p_file);

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
        if (n_nsock <= MAX_CONNECTIONS) {
            if ((clientfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len)) < 0) {
                error("Failed to accept connection");
            }

            if (clientfd_list[n_nsock] != 0) {
                n_nsock++;
            }

            for (int i = 0; i <= n_nsock; i++) {
                if (clientfd_list[i] == 0) {
                    clientfd_list[i] = clientfd;
                    break;
                }
            }

            if (clientfd_list[n_nsock] == 0) {
                n_nsock--;
            }
        }

        arguments.arg1 = clientfd;
        arguments.arg2 = cli_addr;
        arguments.arg3 = &n_nsock;
        arguments.arg4 = clientfd_list;
        arguments.arg5 = p_file;

        pthread_t t;
        pthread_create(&t, NULL, handle_connection, (void*)&arguments);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}