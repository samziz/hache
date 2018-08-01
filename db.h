#ifndef DB_H
#define DB_H

#include <limits.h>


/*** ERRORS ***/

#define E_DB_NOKEY -1
#define E_DB_READ -2
#define E_DB_WRITE -3
#define E_DB_ALLOC -4

/*** SETTINGS ***/

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
int ht_get(HashTable *table, char *key, Entry *en);
int ht_make_table(HashTable *table);
int ht_remove(HashTable *table, char *key);

#endif
