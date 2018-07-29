#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "net.h"
#include "shell.h"

int launch_interactive_shell(char *host, int port)
{
	// Connect to hache service
	int sock = net_client_connect(host, port);

	while (1)
	{
		// Receive user input
	    printf("> ");
	    char *args = readline("");

	    char *cmd = parse_shell_command(args);
	    
	    int res = execute_shell_command(cmd, sock);
	    
	    if (res == 1)
	    {
	    	break;
	    }
	};

	return 0;
}

char *parse_shell_command(char *args)
{	
	char *cmd = strtok(args, " ");
	return cmd;
}

int execute_shell_command(char *cmd, int sock)
{
	if (strcmp(cmd, "EXIT") == 0)
	{
		return 1;
	};

	return 0;
}
