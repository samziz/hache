#include "disk.h"
#include <fcntl.h>
#include <ftw.h>
#include <limits.h>
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

	if (stat(path, &st) == -1) {
		errno = 0;

		if (mkdir(path, S_IRWXU) == 0)
			return 0;

		// Log all likely error codes
		switch (errno) {
			case EACCES:
				printf(">> EACCES error making directory %s\n", path);
				break;
			case ENOENT:
				printf(">> ENOENT error making directory %s\n", path);
				break;
		}

		return E_DISK_FORMAT;
	}

	return 0;
}

int disk_safe_rm(const char *path, const struct stat *stat, int type, struct FTW *ftw)
{
	errno = 0;

	if (type == FTW_D) {
		if (rmdir(path) != 0) {
			// Log all likely error codes
			switch (errno) {
				case EACCES:
					printf(">> EACCES error removing file %s\n", path);
					break;
				case EBUSY:
					printf(">> EBUSY error removing file %s\n", path);
					break;
				case EEXIST:
				case ENOTEMPTY:
					printf(">> EEXIST/ENOTEMPTY error removing file %s\n", path);
					break;
			}

			return E_DISK_RM;
		}
	}

	if (type == FTW_F) {
		if (unlink(path) != 0) {
			// Log all likely error codes
			switch (errno) {
				case EACCES:
					printf(">> EACCES error removing file %s\n", path);
					break;
				case EBUSY:
					printf(">> EBUSY error removing file %s\n", path);
					break;
				case ENOENT:
					printf(">> ENOENT error removing file %s\n", path);
					break;
			}

			return E_DISK_RM;
		};
	}

	else {
		// We don't want to deal with objects that aren't files or directories.
		return E_DISK_RM;
	}

	return 0;
}

int disk_safe_rmdir(char *path)
{	
	if (nftw(path, *disk_safe_rm, 100, FTW_DEPTH) != 0) {
		return E_DISK_RM_DIR;
	};

	return 0;
}


/*** INTERFACE FUNCTIONS ***/

// Remove a service PID and port from the disk service registry. Supply
// zero for PID to indicate no argument (we will never have PID 0).
int disk_deregister_service(int pid, int port)
{
	if (!pid) {
		pid = getpid();
	}
    
    char dirpath[PATH_MAX];
    sprintf(dirpath, "%s/hache/proc/%d", disk_get_homedir(), pid);

    if (disk_safe_rmdir(dirpath) == E_DISK_RM_DIR) {
    	return E_DISK_RM_DIR;
    }

    return 0;
}

// Create hache/ dir with subdirs in var/ if none exists.
int disk_format_env()
{
	char *path = disk_get_homedir();

	strcat(path, "/hache");
	if (disk_safe_mkdir(path) != 0) {
		return E_DISK_FORMAT;
	}

	strcat(path, "/dumps");
	if (disk_safe_mkdir(path) != 0) {
		return E_DISK_FORMAT;
	}
	
	return 0;
}

// Add a service PID and port to the disk service registry. Supply
// zero for PID to indicate no argument (we will never have PID 0).
int disk_register_service(int pid, int port)
{
	if (!pid) {
		pid = getpid();
	}

    char fpath[PATH_MAX];
    FILE *file;

    sprintf(fpath, "%s/hache/proc/%d", disk_get_homedir(), pid);
    disk_safe_mkdir(fpath);
    
    strcat(fpath, "/status");
    
    file = fopen(fpath, "w");
    fprintf(file, "TIME: %u\nPORT: %d\n", (unsigned) time(NULL), port);
    	
    fclose(file);

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

	char buf[10000];
    memset(buf, 0, 10000);

	while (fgets(buf, 10000, file) > 0) {
		char *index = strtok(buf, " ");
		char *key = strtok(buf, " ");
		char *value = strtok(NULL, "\n");

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
	char fname[10000];
	FILE *file;

	sprintf(fname, "%s/%u.hcdump", disk_get_homedir(), (unsigned) time(NULL));

	file = fopen(fname, "w");

	for (unsigned short i = 0; i < USHRT_MAX; i++) {	
		if (table->entries[i].key) {
			fprintf(file, "%d %s %s\n", i, table->entries[i].key, table->entries[i].value);
		}
	}

	fclose(file);
	free(fname);
	return 0;
}

int disk_write_thread(void *ptr)
{
    struct HashTable *table = (struct HashTable *)ptr;

    while (1) {
        disk_write_to_file(table);
        sleep(WRITE_INTERVAL_SECS);
    }

    return 0;
}
