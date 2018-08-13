#include "client.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "config.h"


/*** INTERFACE FUNCTIONS ***/

/* Connect to a hache service at the specified host and port */
int client_connect(char *host, int port)
{
    int sock;
    struct addrinfo *addr;
    struct sockaddr_in serv;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        puts(">> Error opening client socket");
        return 1;
    }

    if (getaddrinfo(host, "http", (void*)NULL, &addr) != 0) {
        puts(">> Error resolving destination hostname");
        return 1;
    }

    for (struct addrinfo *res = addr; res != NULL; res = res->ai_next) {
        if (res->ai_addr->sa_family == AF_INET) {
            struct sockaddr_in* saddr = (struct sockaddr_in*)res->ai_addr;
            memset(&serv, 0, sizeof(serv));
            serv.sin_family = AF_INET;
            serv.sin_port = htons(port);
            serv.sin_addr = saddr->sin_addr;
        }
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
       puts(">> Error creating client socket");
    }

    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) == -1) {
       printf(">> Error connecting to Hache service at %s:%d\n", host, port);
       exit(1);
    }

    return sock;
}