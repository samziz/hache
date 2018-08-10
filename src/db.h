#ifndef DB_H
#define DB_H

#include <limits.h>
#include "config.h"


/*** ERRORS ***/

#define E_DB_NOKEY -1
#define E_DB_READ -2
#define E_DB_WRITE -3
#define E_DB_DELETE -4
#define E_DB_ALLOC -5


/*** TYPES ***/

typedef struct Entry {
	void *key;
	void *value;
	struct Entry *next;
} Entry;

typedef struct HashTable {
	struct Entry entries[USHRT_MAX];
} HashTable;


/*** INTERFACE FUNCTIONS ***/

int ht_add(HashTable *table, char *key, char *value);
int ht_get(HashTable *table, char *key, Entry *en);
int ht_make_table(HashTable *table);
int ht_remove(HashTable *table, char *key);


#endif
