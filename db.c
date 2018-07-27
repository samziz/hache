#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

int ht_add(HashTable *table, char *key, char *value)
{	
	int hash = ht_hash(key);
	table->entries[hash] = value;
	return 0;
}

char *ht_get(HashTable *table, char *key)
{
	int hash = ht_hash(key);
	return table->entries[hash];
}

int ht_hash(char *key)
{
	unsigned long hash = 5381;
    int c;

    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    };

    return hash;
}
