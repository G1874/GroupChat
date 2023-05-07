#include "group_chat.h"

void error(const char* err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

void* handle_sending_msg(void* p_arguments)
{
    int sockfd = *((int*)p_arguments);
    char buffer[BUFFLEN];
    char msg_buffer[MESSAGE_LENGTH];
    fflush(stdin);

    while (1)
    {
        memset(buffer, 0, BUFFLEN);
        memset(msg_buffer, 0, MESSAGE_LENGTH);

        fgets(msg_buffer, MESSAGE_LENGTH, stdin);
        strcat(buffer, "M");
        strcat(buffer, msg_buffer);

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
    char msg_buffer[MESSAGE_LENGTH];
    char client_username[USERNAME_LENGTH];
    fflush(stdout);
    
    while (1)
    {
        memset(buffer, 0, BUFFLEN);
        memset(msg_buffer, 0, MESSAGE_LENGTH);
        memset(client_username, 0, USERNAME_LENGTH);

        if ((msg_len = read(*sockfd, buffer, BUFFLEN)) < 0) {
            error("Failed to read");
        }

        for (int i = 0; i < msg_len; i++) {
            if (buffer[i] != '\n') {
                client_username[i] = buffer[i];
            } else {
                for (int j = 0; j < (msg_len-i); j++) {
                    msg_buffer[j] = buffer[i+j+1];
                }
                break;
            }
        }
        
        printf("\033[0;31m%s\033[0m - %s", client_username, msg_buffer);
    }
}

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr = {0}, cli_addr = {0};
    socklen_t cli_len = sizeof(cli_addr), serv_len = sizeof(serv_addr);
    struct hostent* server;
    pthread_t t;
    int sockfd, port;
    char username[USERNAME_LENGTH];

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

    printf("Enter username: ");
    fgets(username, USERNAME_LENGTH, stdin);
    printf("%c", '\n');

    if (write(sockfd, username, USERNAME_LENGTH) < 0) {
        error("Failed to write");
    }

    pthread_create(&t, NULL, handle_sending_msg, (void*)&sockfd);
    handle_receiving_msg(&sockfd);

    pthread_join(t, NULL);

    close(sockfd);

    return EXIT_SUCCESS;
}