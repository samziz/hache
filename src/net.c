#include "net.h"
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
#include "disk.h"


/*** GLOBALS ***/

struct arg_struct {
    int conn;
    struct HashTable *table;
};

struct net_opts {
    int port;
};


/*** FORWARD DECLARATIONS ***/

int net_client_connect(char *host, int port);
int net_get_opts(int argc, char **argv, struct net_opts *opts);
int net_kill_local_service(char *name);
void net_launch_local_service(int port);
void net_serve(HashTable *table, int port);


/*** CLI COMMAND HANDLER ***/

int net_cmd_handler(int argc, char **argv)
{
    if (argc < 3) {
        puts(">> Hache service module requires a command");
        exit(1);
    }

    char *mod_cmd = argv[2];
    struct net_opts opts = {DEFAULT_PORT};

    if (net_get_opts(argc, argv, &opts) != 0) {
        return E_BAD_OPTS;
    }

    if (!strcasecmp(mod_cmd, "start")) {
        net_launch_local_service(opts.port);
    }

    else if (!strcasecmp(mod_cmd, "stop")) {
        char *name = argv[3];
        net_kill_local_service(name);
    }

    else {
        printf(">> Command '%s' not valid for Hache service module\n", mod_cmd);
    }

    return 0;
};


/*** INTERNAL FUNCTIONS ***/

void net_conn_thread(void *ptr)
{
    struct arg_struct *args = (struct arg_struct *)ptr;
    int conn = args->conn;
    struct HashTable *table = args->table;

    if (conn < 0) {   
        close(conn);
        return;
    }

    char *buf = malloc(10000);
    memset(buf, 0, 10000);

    while (read(conn, buf, 10000) > 0) {

        // Strip trailing newline
        if (buf[strlen(buf)-1] == 10) {
            buf[strlen(buf)-1] = 0;
        }

        char *cmd = strtok(buf, " ");
        char *key = strtok(NULL, " ");

        if (!strcasecmp(cmd, "GET")) {
            struct Entry en;

            if (ht_get(table, key, &en) == E_DB_READ) {
                write(conn, "ERROR: E_DB_READ", 17);
            }

            else {
                write(conn, en.value, 10000);
            }

            continue;
        }

        if (!strcasecmp(cmd, "SET")) {
            char *value = strtok(NULL, " ");

            if (ht_add(table, key, value) == E_DB_WRITE) {
                write(conn, "ERROR: E_DB_WRITE", 18);
            }

            else {
                write(conn, "SUCCESS", 8);
            }

            continue;
        }

        if (!strcasecmp(cmd, "DELETE")) {
            ht_remove(table, key);
            write(conn, "SUCCESS", 8);
            continue;
        }

        write(conn, "ERROR", 6);
    }

    free(buf);
    close(conn);
}

int net_get_opts(int argc, char **argv, struct net_opts *opts)
{
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp("--port", argv[i])) {
            char *port = argv[i+1];
            opts->port = atoi(port);
        }
    };

    return 0;
}

int net_kill_local_service(char *name)
{
    return 0;
}

void net_launch_local_service(int port)
{   
    int pid = fork();

    if (pid == 0) {
        struct HashTable table = {};
        
        if (ht_make_table(&table) == E_DB_ALLOC) {
            puts(">> Not enough space to allocate base array");
            exit(1);
        };

        if (disk_format_env() == E_DISK_FORMAT) {
            puts(">> Could not format disk for local storage");
            exit(1);
        }

        pthread_t disk_thread;
        pthread_create(&disk_thread, NULL, (void *)&disk_write_thread, (void*)&table);

        net_serve(&table, port);
        printf(">> Shutting down service\n");
    }

    else {
        puts(">> Launched database process");
    }

    exit(0);
}

/* Listen on port and return a character buffer */
void net_serve(HashTable *table, int port)
{
    int new_conn, sock;
    struct sockaddr_in serv;
    
    // Open TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock < 0) {
        puts(">> Error opening socket");
        exit(1);
    }

    // Initialise server address
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htons(INADDR_ANY);
    serv.sin_port = htons(port);

    // Bind socket to port specified by user (default 7070)
    if (bind(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        puts(">> Error on binding socket. Run 'ps' to see if you already have a database running.");
        exit(1);
    };
    
    if (listen(sock, 128) < 0) {
        puts(">> Error on listening");
        exit(1);
    };

    while (1) {
        new_conn = accept(sock, (struct sockaddr *)NULL, (socklen_t *)NULL);

        struct arg_struct args;
        args.conn = new_conn;
        args.table = table;

        // Spawn thread to handle connection
        pthread_t io_thread;
        pthread_create(&io_thread, NULL, (void *)&net_conn_thread, (void *)&args);
    };
  
    return;
}


/*** INTERFACE FUNCTIONS ***/

/* Connect to a hache service at the specified host and port */
int net_client_connect(char *host, int port)
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
