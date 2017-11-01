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

void conn_handler(void *arg)
{
    int conn = *((int *)arg);

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
    write_sock_data_to_db(buf);

    write(conn, "SUCCESS", 8);
    close(conn);
}

/* Parse kv data and write to db */
void write_sock_data_to_db(void *ptr)
{
    char *key, *val;

    char *data = ptr;
    int len = sizeof(data) / sizeof(data[0]);

    int i = 0;

    // Read until null byte and write to key
    for (i = 0; i < len; i++) {
        if (data[i] == 0)
        {
            break;
        }

        key[i] = data[i];
    }

    // Read until null byte and write to val
    for (i = i; i < len; i++) {
        if (data[i] == 0)
        {
            break;
        }

        val[i] = data[i];
    }

    // add_pair_to_db(key, val);
}

/* Listen on port and return a character buffer */
void listen_and_write(int port)
{
   	int new_conn, sock;
   	struct sockaddr_in cli, serv;
    
    // Open TCP socket
   	sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf(">> Error opening socket\n");
    }

    // Initialise server address
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htons(INADDR_ANY);
    serv.sin_port = htons(port);

    // Bind socket to port specified by user (default 7070)
    if (bind(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        printf(">> Error on binding socket\n");
    };
    
    if (listen(sock, 128) < 0)
    {
        printf(">> Error on listening\n");
    };

  	while (1)
  	{
        new_conn = accept(sock, (struct sockaddr *)NULL, (socklen_t *)NULL);

        // Spawn thread to handle connection
        pthread_t io_thread;
        pthread_create(&io_thread, NULL, (void *)&conn_handler, (void *)&new_conn);
  	}
  
   	return;
}