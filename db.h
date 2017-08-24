#ifndef DB_H
#define DB_H

// Max DB size in bytes
#define MAX_DB_SIZE 2*1000000000
#define MAX_KEY_SIZE 1028
#define MAX_VAL_SIZE 4096

typedef struct Pair {
	char *key;
	char *val;
	struct Pair* next;
} Pair;

int add_pair_to_db(char *key, char *val);
int deserialise(char *buf, Pair *f);
int read_until_null_byte(char *buf, char *s, int i);
int read_val(char *key, char *val, char *buf);
int remove_pair(char *key);
int serialise(Pair *f, char *buf);

#endif