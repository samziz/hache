#include "service.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "disk.h"


/*** GLOBALS ***/

struct arg_struct {
    int conn;
    struct HashTable *table;
};

struct service_opts {
    int port;
};


/*** FUNCTION PROTOTYPES ***/

int service_get_opts(int argc, char **argv, struct service_opts *opts);
int service_kill_local(char *name);
void service_launch_local(int port);
void service_serve(HashTable *table, int port);


/*** CLI COMMAND HANDLER ***/

int service_cmd_handler(int argc, char **argv)
{
    if (argc < 3) {
        puts(">> Hache service module requires a command");
        exit(1);
    }

    char *mod_cmd = argv[2];
    struct service_opts opts = {DEFAULT_PORT};

    if (service_get_opts(argc, argv, &opts) != 0) {
        return E_BAD_OPTS;
    }

    if (!strcasecmp(mod_cmd, "start")) {
        service_launch_local(opts.port);
    }

    else if (!strcasecmp(mod_cmd, "stop")) {
        char *name = argv[3];
        service_kill_local(name);
    }

    else {
        printf(">> Command '%s' not valid for Hache service module\n", mod_cmd);
    }

    return 0;
};


/*** INTERNAL FUNCTIONS ***/

void service_conn_handler(void *ptr)
{
    struct arg_struct *args = (struct arg_struct *)ptr;
    int conn = args->conn;
    struct HashTable *table = args->table;

    if (conn < 0) {   
        close(conn);
        return;
    }

    char buf[MAX_ENTRY_SIZE];
    memset(buf, 0, MAX_ENTRY_SIZE);

    while (read(conn, buf, MAX_ENTRY_SIZE) > 0) {

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
                char *value = (char*)en.value;
                write(conn, value, strlen(value));
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

    close(conn);
}

int service_get_opts(int argc, char **argv, struct service_opts *opts)
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

void service_launch_local(int port)
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

        service_serve(&table, port);
        printf(">> Shutting down service\n");
    }

    else {
        puts(">> Launched database process");
    }

    exit(0);
}

/* Listen on port and return a character buffer */
void service_serve(HashTable *table, int port)
{
    int new_conn, sock;
    struct sockaddr_in serv;
    
    // Open TCP socket    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
        struct arg_struct args = { new_conn, table };

        // Spawn thread to handle connection
        pthread_t io_thread;
        pthread_create(&io_thread, NULL, (void *)&service_conn_handler, (void *)&args);
    };
  
    return;
}


/*** INTERFACE FUNCTIONS ***/

int service_kill_local(char *name)
{
    return 0;
}

