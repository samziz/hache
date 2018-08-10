#ifndef CONFIG_H
#define CONFIG_H


// Table config

#define MAX_ENTRY_CHILDREN 50
#define MAX_ENTRY_SIZE 10000
#define MAX_KEY_SIZE 1000
#define MAX_VALUE_SIZE (MAX_ENTRY_SIZE - MAX_KEY_SIZE)
#define TABLE_SIZE (USHRT_MAX * sizeof(struct Entry))


// Networking config

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 7070


// Disk config

#define WRITE_INTERVAL_SECS 20

#endif