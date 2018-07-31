#ifndef DB_H
#define DB_H

#include <limits.h>

#define MAX_CHILDREN 50
#define TABLE_SIZE (USHRT_MAX * sizeof(struct Entry))

typedef struct Entry {
	char *key;
	char *value;
	struct Entry *next;
} Entry;

typedef struct HashTable {
	struct Entry entries[USHRT_MAX];
} HashTable;

int ht_add(HashTable *table, char *key, char *value);
Entry ht_get(HashTable *table, char *key);
HashTable *ht_make_table();
int ht_remove(HashTable *table, char *key);

#endif
