#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "db.h"
#include "net.h"

struct arg_struct {
    int conn;
    struct HashTable *table;
};

// Forward declarations of non-interface functions
void write_sock_data_to_db(HashTable *table, char *data);

void conn_handler(void *ptr)
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

    if (read(conn, buf, 10000) < 0) {
        write(conn, "ERROR", 6);
        close(conn);
        return;
    }

    // Use separate thread to write 
    write_sock_data_to_db(table, buf);

    write(conn, "SUCCESS", 8);
    close(conn);
}

/* Parse kv data and write to db */
void write_sock_data_to_db(HashTable *table, char *data)
{
    char key[MAX_KEY_LENGTH];
    char val[MAX_VALUE_LENGTH];

    int len = sizeof(data) / sizeof(data[0]);

    int i = 0;

    // Read until null byte and write to key
    for (i = 0; i < len; i++) {
        if (data[i] == 0)
            break;

        key[i] = data[i];
    }

    // Read until null byte and write to val
    for (; i < len; i++) {
        if (data[i] == 0)
            break;

        val[i] = data[i];
    }

    ht_add(table, key, val);
}

int net_request(char *host, int port)
{
    int sock;
    struct addrinfo *addr;
    struct sockaddr_in serv;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf(">> Error opening client socket\n");
        return 1;
    }

    if ((addrinfo = getaddrinfo(host)) == NULL)
    {
        printf(">> Error resolving destination hostname\n");
        return 1;
    }

    memset(&serv, 0, sizeof(serv))
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr = addr;

    
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
        pthread_create(&io_thread, NULL, (void *)&conn_handler, (void *)&args);
  	};
  
   	return;
}
