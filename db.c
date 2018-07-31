#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"


/*** INTERNAL FUNCTIONS ***/

// Modification of djb2 hashing algorithm to return within int bounds.
unsigned short ht_hash(char *key)
{
	unsigned short hash = 5381;
    int c;

    while ((c = *key++))
    {
        hash = (hash << 5) + c;
    };

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

Entry ht_get(HashTable *table, char *key)
{
	int hash = ht_hash(key);
	Entry en = table->entries[hash];
	
	while (en.key)
	{
		if (en.key == key)
			return en;

		en = *en.next;
	}

	// FIXME Handle case where no key matches
	return en;
}

HashTable *ht_make_table()
{
	printf(">> Allocating base array of size %luMB\n", TABLE_SIZE/10^6);
	
	HashTable *table = (struct HashTable*) malloc(TABLE_SIZE);
	memset(table, 0, TABLE_SIZE);

	if (table == NULL)
	{
		printf(">> Not enough space to allocate base array\n");
		exit(1);
	}

	return table;
}

int ht_remove(HashTable *table, char *key)
{
	// FIXME: Deal with array pointers allocated in collision cases

	int hash = ht_hash(key);
	Entry new = { NULL, NULL };

	table->entries[hash] = new;

	return 0;
}
