#ifndef SERVICE_H
#define SERVICE_H

#include "config.h"
#include "db.h"


/*** ERRORS ***/

#define E_NO_CMD -1
#define E_BAD_CMD -2
#define E_BAD_OPTS -3


/*** INTERFACE FUNCTIONS ***/

int service_cmd_handler(int argc, char **argv);
int service_kill_local(char *name);

#endif
