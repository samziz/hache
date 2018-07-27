#ifndef NET_H
#define NET_H

#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_LENGTH 10000

int net_request(char *host, int port);
void net_serve(HashTable *table, int port);

#endif
