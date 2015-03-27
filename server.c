#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/errno.h>
#include <pthread.h>

#define BUFFER_LENGTH 1024

void error() {
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
}

#define DEBUG(s) do {\
    fprintf(stderr, "%d: %s\n", __LINE__, s);\
    } while(0)

void handle_client(void *data) {
    DEBUG("handle client");
    int client = (int) data;
    char message[BUFFER_LENGTH];
    ssize_t length;
    for (; ;) {
        length = recv(client, &message, BUFFER_LENGTH, 0);
        if (length <= 0) {
            break;
        }
        DEBUG("message:");
        DEBUG(message);
        length = send(client, &message, length, 0);
        if (length < 0) {
            break;
        }
    }
    close(client);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: <hostname> <port>");
        exit(1);
    }

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        error();
    }

    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error();
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);
    address.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        close(sock);
        error();
    }

    int optvalue = 1;
    socklen_t optlength = sizeof(optvalue);
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optvalue, optlength) == -1) {
        close(sock);
        error();
    }

    if (listen(sock, SOMAXCONN) < 0) {
        close(sock);
        error();
    }

    fprintf(stderr, "Server started on http://%s:%d\n", argv[1], port);

    struct sockaddr in_addr;
    socklen_t in_len = sizeof(in_addr);

    for (; ;) {
        memset(&in_addr, 0, in_len);
        int client = accept(sock, &in_addr, &in_len);
        DEBUG("new client");
        if (client < 0) {
            close(sock);
            error();
        }
        pthread_t thread;
        pthread_create(&thread, NULL, (void *) &handle_client, (void *) client);
    }
}