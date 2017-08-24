#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "disk.h"

/*
	Writing to disk doesn't use any kind of diffing
	mechanism right now. It should only be used on
	a SIGKILL, or very rarely to back up data.
*/

void write_to_file(Pair *f)
{
	char *path = getenv(path);
	FILE *file = fopen(path, "rb");

	char *buf;
	serialise(f, buf);
	
	fwrite((void *)&buf, 1, sizeof(buf), file);
}

void read_from_file(char *buf)
{
	char *path = getenv(path);
	FILE *file = fopen(path, "rb");

	fread((void *)&buf, 1, MAX_DB_SIZE, file);
}