#include "disk.h"
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <time.h>
#include <unistd.h>


/*** INTERNAL FUNCTIONS ***/

char *disk_get_homedir()
{
	struct passwd *pw = getpwuid(getuid());
	return (char*) pw->pw_dir;
}

int disk_safe_mkdir(char *path)
{
	static struct stat st = {0};

	if (stat(path, &st) == -1)
	{
		errno = 0;

		if (mkdir(path, S_IRWXU) == 0)
			return 0;

		switch (errno)
		{
			case EACCES:
				printf(">> EACCES error making directory %s\n", path);
				break;
			case ENAMETOOLONG:
				printf(">> ENAMETOOLONG error making directory %s\n", path);
				break;
			case ENOENT:
				printf(">> ENOENT error making directory %s\n", path);
				break;
			case ENOTDIR:
				printf(">> ENOTDIR error making directory %s\n", path);
				break;
			case EROFS:
				printf(">> EROFS error making directory %s\n", path);
				break;
		}

		return E_DISK_FORMAT;
	}

	return 0;
}


/*** INTERFACE FUNCTIONS ***/

// Create hache/ dir with subdirs in var/ if none exists.
int disk_format_env()
{
	char *path = disk_get_homedir();

	strcat(path, "/hache");
	if (disk_safe_mkdir(path) != 0)
		return E_DISK_FORMAT;

	strcat(path, "/dumps");
	if (disk_safe_mkdir(path) != 0)
		return E_DISK_FORMAT;
	
	return 0;
}

// Reads the most recent dump in ~/hache/dumps/ into memory.
HashTable *disk_restore_from_file()
{
	char *fname;
	HashTable *table;
	FILE *file;

	table = (struct HashTable*) malloc(sizeof(struct HashTable));

	file = fopen(fname, "r");

	char *buf = malloc(10000);
    memset(buf, 0, 10000);

	while (fgets(buf, 10000, file) > 0)
	{
		char *index = strtok(buf, " ");
		char *key = strtok(buf, " ");
		char *value = strtok(NULL, " ");

		int i = atoi(index);

		Entry en = {key, value};
		table->entries[i] = en;

		memset(buf, 0, 10000);
	}

	fclose(file);
	return table;
}

// Writes in-memory data to ~/hache/dumps/ in case of crash.
int disk_write_to_file(HashTable *table)
{
	char *fname = (char*)malloc(sizeof(char) * 100);
	FILE *file;

	sprintf(fname, "%s/%u.hcdump", disk_get_homedir(), (unsigned) time(NULL));

	file = fopen(fname, "w");

	for (unsigned short i = 0; i < USHRT_MAX; i++)
	{	
		if (table->entries[i].key)
			fprintf(file, "%d %s %s\n", i, table->entries[i].key, table->entries[i].value);
	}

	fclose(file);

	return 0;
}

int disk_write_thread(void *ptr)
{
    struct HashTable *table = (struct HashTable *)ptr;

    if (disk_format_env() != 0)
    	return E_DISK_FORMAT; /* should call pthread_exit */

    while (1)
    {
        disk_write_to_file(table);
        sleep(WRITE_INTERVAL_SECS);
    }

    return 0;
}