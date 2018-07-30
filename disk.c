#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "disk.h"


// Creates a /hache/ folder in the root directory if one
// doesn't already exist.
int disk_format_env()
{
	return 0;
};

// Reads data from disk into memory.
HashTable *disk_read_serialised_table()
{

};

// Writes in-memory data to disk in case of crash. Requires
// a /hache/ folder to be created, usually by disk_format_env.
int disk_write_serialised_table(HashTable *table)
{
	return 0;
};