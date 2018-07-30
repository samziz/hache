#ifndef DISK_H
#define DISK_H

int disk_format_env();
HashTable *disk_read_serialised_table();
int disk_write_serialised_table(HashTable *table);

#endif
