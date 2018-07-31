#ifndef DISK_H
#define DISK_H

#include "db.h"

#define WRITE_INTERVAL_SECS 20

int disk_format_env();
HashTable *disk_restore_from_file();
int disk_write_to_file(HashTable *table);
int disk_write_thread(void *ptr);

#endif
