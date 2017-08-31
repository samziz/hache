#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

void sigint_handler(int sig)
{
	printf("\n>> Wrapping up...\n");
	exit(0);
}

int main() {
	// Handle signals
	signal(SIGINT, sigint_handler);
	
	listen_and_write(7070);
	return 0;
}