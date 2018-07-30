#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "main.h"
#include "net.h"
#include "shell.h"


void execute_command(char *cmd)
{
	if (!strcmp(cmd, "start"))
	{
		net_launch_local_service();
	}

	if (!strcmp(cmd, "shell"))
	{
		// FIXME: Parse host and port from CLI flags
		sh_launch_interactive("localhost", 7070);
	}

	else
	{
		printf(">> Invalid command '%s'\n", cmd);
	}
}

int main(int argc, char *argv[]) 
{
	if (argc == 1)
	{
		// FIXME: Parse host and port from CLI flags
		sh_launch_interactive("localhost", 7070);
	}

	if (argc == 2)
	{
		char *cmd = argv[1];
		execute_command(cmd);
	}

	return 0;
}
