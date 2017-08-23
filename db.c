#include <stdio.h>
#include <string.h>
#include "buf.h"
#include "db.h"

/* Declare db and last kv pair as global var */
Pair first;
Pair last;


// Add value to end of linked list
int add_pair_to_db(char *key, char *val)
{
	Pair i = first;
	Pair c = { key, val };

	while (1) {
		if (!i.next)
		{
			i.next = &c;
			return 0;
		}

		i = *i.next;
	};
}

// Try to retrieve key and write to buffer provided
// If not found, return status code 1; else return 0
int read_val_to_buf(char *key, char *val, Buf *b)
{
	Pair i = first;

	while (1) {
		if (strcmp(i.key, key) == 0)
		{
			(*b).data = i.key;
			return 0;
		}

		if (!i.next)
		{
			return 1;
		}

		i = *i.next;
	}
}
