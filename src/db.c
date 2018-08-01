#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"


/*** INTERNAL FUNCTIONS ***/

// Hashing algorithm designed for uniform and unpredictable hash distribution.
unsigned short ht_hash(char *key)
{
	unsigned short hash = 0;
	int c;
	int i = 0;

    while ((c = *key++))
    {
    	hash += c << hash;
    	i++;
    }

    hash ^= (hash + i) >> 11;

    return hash;
}


/*** INTERFACE FUNCTIONS ***/

int ht_add(HashTable *table, char *key, char *value)
{
	unsigned short hash = ht_hash(key);

	Entry en = { key, value };
	table->entries[hash] = en;

	return 0;
}

unsigned short ht_count(HashTable *table)
{
	unsigned short count = 0;

	for (unsigned short i = 0; i < USHRT_MAX; i++)
	{
		if (strlen(table->entries[i].key) != 0)
			count++;
	}

	return count;
}

int ht_get(HashTable *table, char *key, Entry *en)
{
	unsigned short hash = ht_hash(key);

	struct Entry en_iter = table->entries[hash];
	
	while (en_iter.key)
	{
		if (!strcmp(en_iter.key, key))
		{
			en->value = en_iter.value;
			return 0;
		}

		en_iter = *en_iter.next;
	}

	return E_DB_READ;
}

int ht_make_table(HashTable *table)
{
	printf(">> Allocating base array of size %luMB\n", TABLE_SIZE/10^6);
	
	table = (struct HashTable*) malloc(TABLE_SIZE);
	memset(table, 0, TABLE_SIZE);

	if (table == NULL)
		return E_DB_ALLOC;

	return 0;
}

int ht_remove(HashTable *table, char *key)
{
	// FIXME: Deal with array pointers allocated in collision cases

	int hash = ht_hash(key);
	Entry new = { NULL, NULL };

	table->entries[hash] = new;

	return 0;
}
