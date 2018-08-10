#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*** INTERNAL FUNCTIONS ***/

// Hashing algorithm designed for uniform and unpredictable hash distribution.
unsigned short ht_hash(char *key)
{
	unsigned short hash = 0;
	int c;
	int i = 0;

    while ((c = *key++)) {
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
	const size_t key_size = sizeof(key) / sizeof(key[0]);
	const size_t value_size = sizeof(value) / sizeof(value[0]);

	void *key_ptr = malloc(key_size);
	void *value_ptr = malloc(value_size);

	memcpy(key_ptr, (void*)key, key_size);
	memcpy(value_ptr, (void*)value, value_size);

	Entry en = { key_ptr, value_ptr };

	table->entries[hash] = en;

	return 0;
}

unsigned short ht_count(HashTable *table)
{
	unsigned short count = 0;

	for (unsigned short i = 0; i < USHRT_MAX; i++) {
		if (strlen(table->entries[i].key) != 0) {
			count++;
		}
	}

	return count;
}

int ht_get(HashTable *table, char *key, Entry *en)
{
	unsigned short hash = ht_hash(key);

	struct Entry en_iter = table->entries[hash];
	
	while (en_iter.key) {
		if (!strcmp((char*)en_iter.key, key)) {
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

	if (table == NULL) {
		return E_DB_ALLOC;
	}

	return 0;
}

int ht_remove(HashTable *table, char *key)
{
	unsigned short hash = ht_hash(key);

	struct Entry *en = &table->entries[hash];
	
	while (en->key) {
		if (!strcmp((char*)en->key, key)) {
			free(en->key);
			free(en->value);
			memset(en, 0, sizeof(*en));
			return 0;
		}

		en = en->next;
	}

	return E_DB_DELETE;
}
