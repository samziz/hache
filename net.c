#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "buf.h"
#include "db.h"
#include "net.h"

/* Parse kv data and write to db */
void write(void *ptr)
{
    char *key, *val;

    char *data = (char*)ptr;
    int len = sizeof(data) / sizeof(data[0]);

    int i = 0;

    for (i = 0; i < len; i++) {
        if (data[i] == 0)
        {
            break;
        }

        key[i] = data[i];
    }

    for (i = i; i < len; i++) {
        if (data[i] == 0)
        {
            break;
        }

        val[i] = data[i];
    }

    add_pair_to_db(key, val);
}

/* Listen on port and return a character buffer */
char *listen_and_write(int port)
{
    Buf buf;
    memset(buf.data, 0, MAX_BUF_SIZE);

   	int len, sock;
   	struct sockaddr_in dest; 
    pthread_t write_thread;
 
   	sock = socket(AF_INET, SOCK_STREAM, 0);
  
   	memset(&dest, 0, sizeof(dest));
   	dest.sin_family = AF_INET;
   	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   	dest.sin_port = htons(port);
 
   	connect(sock, (struct sockaddr *)&dest, sizeof(struct sockaddr_in{}));
  	
  	while (1) 
  	{
  		recv(sock, buf.data, MAX_BUF_SIZE, 0);

      // Use separate thread to write 
      pthread_create(&write_thread, NULL, (void *)&write, (void *)&buf.data);
      write_thread = 0;
  	}
  
   	return 0;
}