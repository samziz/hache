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
		launch_interactive_shell();
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
		int pid = fork();
		
		if (pid == 0) 
		{
			HashTable table;			
			net_serve(&table, 7070);
		} 
		else 
		{
			printf(">> Launched database process\n");
			exit(0);
		}
	}
}
