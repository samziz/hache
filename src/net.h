#ifndef NET_H
#define NET_H

#include "db.h"


/*** ERRORS ***/

#define E_NO_CMD -1
#define E_BAD_CMD -2
#define E_BAD_OPTS -3


/*** SETTINGS ***/

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 7070
#define MAX_KEY_LENGTH 1000
#define MAX_VALUE_LENGTH 10000


/*** INTERFACE FUNCTIONS ***/

int net_client_connect(char *host, int port);
int net_cmd_handler(int argc, char **argv);
int net_kill_local_service(char *name);

#endif
