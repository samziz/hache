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

// disk_fread reads from a binary file into a section of memory (ptr)
// until it encounters a specific series of bytes (terminator). It
// then returns the number of bytes read.
int disk_fread(void *ptr, const char *terminator, int offset, FILE *file)
{
	char buf[MAX_ENTRY_SIZE];
	
	int count = 0;
	const int TERM_LEN = strlen(terminator);

	char *match = (char*) calloc(TERM_LEN, sizeof(char));
	int match_ind = 0;

	// Apply offset from beginning of file
	fseek(file, offset, SEEK_CUR);

	while (!match[TERM_LEN-1]) {
		fread(&buf, 1, 1, file);
		count += 1;

		if (buf[count] == terminator[match_ind]) {
			match[match_ind] = buf[count];
			match_ind += 1;
		}
	}

	return 0;
}

int disk_safe_mkdir(char *path)
{
	struct stat st = {0};

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
	FILE *file;
	HashTable *table;
	int offset = 0;

	table = (struct HashTable*) malloc(sizeof(struct HashTable));

	file = fopen(fname, "rb");

	char buf[MAX_ENTRY_SIZE];
    memset(buf, 0, MAX_ENTRY_SIZE);

	while (fgets(buf, MAX_ENTRY_SIZE, file) > 0) {
		char key[MAX_KEY_SIZE];
		offset += disk_fread(key, BIN_TERM, offset, file);

		char value[MAX_VALUE_SIZE];
		offset += disk_fread(value, BIN_TERM, offset, file);

		ht_add(table, key, value);

		memset(buf, 0, MAX_ENTRY_SIZE);
	}

	fclose(file);
	return table;
}

// Writes in-memory data to ~/hache/dumps/ in case of crash.
int disk_write_to_file(HashTable *table)
{
	char fname[MAX_ENTRY_SIZE];
	FILE *file;

	sprintf(fname, "%s/%u.hcdump", disk_get_homedir(), (unsigned) time(NULL));

	file = fopen(fname, "wb");

	for (unsigned short i = 0; i < USHRT_MAX; i++) {	
		if (table->entries[i].key) {
			char *key = (char*)table->entries[i].key;	
			char *value = (char*)table->entries[i].value;
			char str[MAX_ENTRY_SIZE];
			
			sprintf(str, "%s%s%s%s", key, BIN_TERM, value, BIN_TERM);
			fwrite(str, sizeof(char), sizeof(str), file);
		}
	}

	fclose(file);
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
