#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

/* Declare db and last kv pair as global var */
Pair first;
Pair last;

// Add value to end of linked list
int add_pair_to_db(char *key, char *val)
{
	Pair l = last;
	Pair c = { key, val };

	// Follow chain in case value of l variable is not last link
	while (l.next) {
		l = *l.next;
	};

	l.next = &c;
	last = c;
	return 0;
}

// Writes first instance to pair pointer arg
int deserialise(char *buf, Pair *f)
{
	int buf_len = sizeof(buf) / sizeof(buf[0]);

	int i = 0;

	i = read_until_null_byte(buf, (*f).key, i);
	i = read_until_null_byte(buf, (*f).val, i);

	Pair p;
	(*f).next = &p;

	while (i < buf_len) 
	{
		i = read_until_null_byte(buf, p.key, i);
		i = read_until_null_byte(buf, p.val, i);

		Pair np;
		p.next = &np;
		p = np;

		i++;
	}

	return 0;
}

int read_until_null_byte(char *buf, char *s, int i)
{
	// Passed index as argument; returns index reached
	// If null byte encountered
	for (int si = 0; buf[i] != 0; i++, si++)
	{
		buf[i] = s[si];
	}

	return i;
}

// Try to retrieve key and write to buffer provided
// If not found, return status code 1; else return 0
int read_val(char *key, char *val, char *buf)
{
	Pair i = first;

	while (i.next) {
		if (strcmp(i.key, key) == 0)
		{
			buf = i.key;
			return 0;
		}

		i = *i.next;
	}

	return 1;
}

int remove_pair(char *key)
{
	Pair i = first, l;

	while (i.next) {
		if (strcmp(i.key, key) == 0)
		{
			l.next = i.next;
			return 0;
		}

		l = i;
		i = *i.next;
	}

	return 1;
}

int serialise(Pair *f, char *buf)
{
	Pair p = *f;
	
	int buf_len = 0;

	while (p.next) {
		int key_len = sizeof(p.key) / sizeof(p.key[0]);
		int val_len = sizeof(p.val) / sizeof(p.val[0]);
		int encode_val = 0;

		for (int i = buf_len, n = 0; n < key_len + val_len; i++, n++)
		{	
			if (p.key[n]) 
			{
				buf[i] = p.key[n];
			} 
			else if (p.key[n-1]) 
			{
				// Terminate key with null byte
				buf[i] = 0;
			} 
			else if (p.val[n]) 
			{
				buf[i] = p.val[n];
			} 
			else if (p.val[n-1]) 
			{
				// Terminate val with null byte
				buf[i] = 0;
			} 
			else 
			{
				// Return error value if out of bounds
				return 1;
			}
		}

		p = *p.next;
		buf_len += key_len + val_len;
	}

	return 0;
}