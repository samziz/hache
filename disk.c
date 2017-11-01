#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "disk.h"

/*
	Writing to disk doesn't use any kind of diffing
	mechanism right now. It should only be used on
	a SIGKILL, or very rarely to back up data.
*/

