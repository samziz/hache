#ifndef DISK_H
#define DISK_H

#include "db.h"

/*** ERRORS ***/

#define E_DISK_DATACORRUPTED -1
#define E_DISK_FORMAT -2

/*** SETTINGS ***/

#define WRITE_INTERVAL_SECS 20


/*** INTERFACE FUNCTIONS ***/

int disk_deregister_service(int pid, int port);
int disk_format_env();
int disk_register_service(int pid, int port);
HashTable *disk_restore_from_file();
int disk_write_to_file(HashTable *table);
int disk_write_thread(void *ptr);


#endif
