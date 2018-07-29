#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "db.h"
#include "main.h"
#include "net.h"
#include "shell.h"

void sigint_handler(int sig)
{
	printf("\n>> Wrapping up...\n");
	exit(0);
}

int main(int argc, char *argv[]) 
{
	// Handle signals
	signal(SIGINT, sigint_handler);

	if (argc == 1)
	{
		// FIXME: Parse host and port from CLI flags
		launch_interactive_shell("127.0.0.1", 7070);
	}

	if (argc == 2)
	{
		char *cmd = argv[1];
		execute_command(cmd);
	}

	return 0;
}

void execute_command(char *cmd)
{
	if (strcmp(cmd, "start") == 0)
	{
		launch_local_service();
	}

	else
	{
		printf(">> Invalid command '%s'\n", cmd);
	}
}

void launch_local_service()
{
	int pid = fork();
	
	if (pid == 0) 
	{
		HashTable *table = ht_make_table();
		net_serve(table, 7070);
		printf(">> Shutting down service\n");
	}

	else 
	{
		printf(">> Launched database process\n");
		exit(0);
	}
}
