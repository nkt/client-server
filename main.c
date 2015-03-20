#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_LENGTH 256

void error(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        error("Usage: <hostname> <port>");
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("Could not create socket");
    }

    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("Hostname not resolved");
    }

    int port = atoi(argv[2]);
    if (port == 0) {
        error("Bad port");
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);
    address.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
        error("Connect failed");
    }

    printf("Successful connected to %s:%d\n\n", argv[1], port);

    char buffer[BUFFER_LENGTH];
    long length;
    for (; ;) {
        printf(" > ");
        memset(buffer, 0, BUFFER_LENGTH);
        scanf("%s", buffer);
        length = write(sock, buffer, BUFFER_LENGTH);
        if (length < 0) {
            error("Could not write to socket");
        }
        memset(buffer, 0, BUFFER_LENGTH);
        length = read(sock, buffer, BUFFER_LENGTH - 1);
        if (length < 0) {
            error("Could not read to socket");
        } else if (length != 0) {
            printf("%s\n", buffer);
        }
    }
}