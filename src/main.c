#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "main.h"
#include "net.h"
#include "shell.h"

// Parse the second word of a command and pass args
// along to the relevant module to be handled.
void execute_command(int argc, char **argv)
{
	if (!strcmp(argv[1], "service")) {
		if (!net_cmd_handler(argc, argv)) {
			exit(1);
		}
	}

	else if (!strcmp(argv[1], "shell")) {
		if (!sh_cmd_handler(argc, argv)) {
			exit(1);
		}
	}

	else {
		printf(">> Invalid command '%s'\n", argv[1]);
	}
}

int main(int argc, char *argv[]) 
{
	if (argc > 1) {
		execute_command(argc, argv);
	}

	else {
		printf(">> Please supply a command\n");
	}
}
