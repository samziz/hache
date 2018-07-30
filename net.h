#ifndef NET_H
#define NET_H

#include "db.h"

#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_LENGTH 10000

int net_client_connect(char *host, int port);
int net_cmd_handler(int argc, char **argv);
int net_kill_local_service(char *name);

#endif
