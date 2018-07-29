#ifndef DB_H
#define DB_H

#include <limits.h>

#define MAX_CHILDREN 50

typedef struct Entry {
	char *key;
	char *value;
	struct Entry *children;
} Entry;

typedef struct HashTable {
	struct Entry entries[INT_MAX];
} HashTable;

int ht_add(HashTable *table, char *key, char *value);
Entry ht_get(HashTable *table, char *key);
int ht_hash(char *key);
HashTable *ht_make_table();
int ht_remove(HashTable *table, char *key);
Entry ht_traverse_children(Entry *entries, char *key);

#endif
