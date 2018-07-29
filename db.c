#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

int ht_add(HashTable *table, char *key, char *value)
{	
	int hash = ht_hash(key);
	Entry en = { key, value };
	table->entries[hash] = en;
	return 0;
}

Entry ht_get(HashTable *table, char *key)
{
	int hash = ht_hash(key);
	Entry en = table->entries[hash];
	
	return en;
}

// Modification of djb2 hashing algorithm to return within int bounds.
int ht_hash(char *key)
{
	unsigned long hash = 5381;
    int c;

    while ((c = *key++))
    {
        hash = (hash << 5) + c;
    };

    return hash;
}

HashTable *ht_make_table()
{
	HashTable *table = (struct HashTable*) malloc(sizeof(struct HashTable));
	memset(table->entries, 0, sizeof(table->entries) / sizeof(Entry));
	
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

Entry ht_traverse_children(Entry *entries, char *key) 
{
	Entry en;
	int i;
	int len = sizeof entries / sizeof entries[0];

	for (i = 0; i < len; i++)
	{
		en = entries[i];
		if (en.key == key)
		{
			return en;
		}
	}

	return en;
	//FIXME handle case where entry not in array
	//Right now will cause segfault
}
