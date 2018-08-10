#ifndef DISK_H
#define DISK_H

#include "config.h"
#include "db.h"

/*** ERRORS ***/

#define E_DISK_DATACORRUPTED -1
#define E_DISK_FORMAT -2
#define E_DISK_RM -3
#define E_DISK_RM_DIR -4
#define E_DISK_MK -5
#define E_DISK_MK_DIR -6
#define E_DISK_NOT_A_FILE -7


/*** INTERFACE FUNCTIONS ***/

int disk_deregister_service(int pid, int port);
int disk_format_env();
int disk_register_service(int pid, int port);
HashTable *disk_restore_from_file();
int disk_write_to_file(HashTable *table);
int disk_write_thread(void *ptr);


#endif
