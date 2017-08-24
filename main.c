#include <stdio.h>
#include "main.h"

int main() {
	// Handle signals
	signal(SIGINT, inthandler);

	listen_and_write(7070);
	return 0;
}

void int_handler(int sig)
{
	db.
}