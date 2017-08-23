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

int read_val_to_buf(char *key, char *val, Buf *b);
int add_pair_to_db(char *key, char *val);

#endif