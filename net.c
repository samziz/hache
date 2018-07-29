#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "net.h"

struct arg_struct {
    int conn;
    struct HashTable *table;
};


/* Connect to a hache service at the specified host and port */
int net_client_connect(char *host, int port)
{
    int sock;
    struct addrinfo *addr;
    struct sockaddr_in serv;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf(">> Error opening client socket\n");
        return 1;
    }

    if (getaddrinfo(host, "http", (void*)NULL, &addr) != 0)
    {
        printf(">> Error resolving destination hostname\n");
        return 1;
    }

    for (struct addrinfo *res = addr; res != NULL; res = res->ai_next)
    {
        if (res->ai_addr->sa_family == AF_INET)
        {
            struct sockaddr_in* saddr = (struct sockaddr_in*)res->ai_addr;
            memset(&serv, 0, sizeof(serv));
            serv.sin_family = AF_INET;
            serv.sin_port = htons(port);
            serv.sin_addr = saddr->sin_addr;
        }
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
       printf(">> Error creating client socket\n");
    }

    if (connect(sock, (struct sockaddr*)&serv, sizeof(serv)) == -1) {
       printf(">> Error connecting client socket\n");
    }

    return sock;
}

void net_conn_handler(void *ptr)
{
    struct arg_struct *args = (struct arg_struct *)ptr;
    int conn = args->conn;
    struct HashTable *table = args->table;

    if (conn < 0)
    {   
        close(conn);
        return;
    }

    char *buf = malloc(10000);
    memset(buf, 0, 10000);

    while (read(conn, buf, 10000) > 0)
    {
        // Strip trailing newline
        if (buf[strlen(buf)-1] == 10)
            buf[strlen(buf)-1] = 0;

        char *cmd = strtok(buf, " ");
        char *key = strtok(NULL, " ");

        if (strcmp(cmd, "GET") == 0)
        {
            Entry en = ht_get(table, key);
            write(conn, en.value, 10000);
            continue;
        }

        if (strcmp(cmd, "SET") == 0)
        {
            char *value = strtok(NULL, " ");
            ht_add(table, key, value);
            write(conn, "SUCCESS", 8);
            continue;
        }

        if (strcmp(cmd, "DELETE") == 0) 
        {
            ht_remove(table, key);
            write(conn, "SUCCESS", 8);
            continue;
        }

        write(conn, "ERROR", 6);
    }

    close(conn);
}

/* Listen on port and return a character buffer */
void net_serve(HashTable *table, int port)
{
    int new_conn, sock;
    struct sockaddr_in serv;
    
    // Open TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf(">> Error opening socket\n");
        exit(1);
    }

    // Initialise server address
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htons(INADDR_ANY);
    serv.sin_port = htons(port);

    // Bind socket to port specified by user (default 7070)
    if (bind(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        printf(">> Error on binding socket. Run 'ps' to see if you already have a database running.\n");
        exit(1);
    };
    
    if (listen(sock, 128) < 0)
    {
        printf(">> Error on listening\n");
        exit(1);
    };

    while (1)
    {
        new_conn = accept(sock, (struct sockaddr *)NULL, (socklen_t *)NULL);

        struct arg_struct args;

        args.conn = new_conn;
        args.table = table;

        // Spawn thread to handle connection
        pthread_t io_thread;
        pthread_create(&io_thread, NULL, (void *)&net_conn_handler, (void *)&args);
    };
  
    return;
}
