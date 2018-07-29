#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include "net.h"
#include "shell.h"

// Store socket variable globally in case of sigint 
int glob_sock;

void sigint_handler(int sig);

int execute_shell_command(char *args, int sock)
{
	char *cmd = strtok(args, " ");

	if (strcmp(cmd, "EXIT") == 0)
	{
		return 1;
	};

	if (strcmp(cmd, "SET") == 0)
	{
		char *key = strtok(NULL, " ");
		char *value = strtok(NULL, " ");
		char query[10000];
		sprintf(query, "SET %s %s", key, value);

		write(sock, query, strlen(query));

		char buf[10000];
		read(sock, buf, 10000);

		printf("%s\n", buf);
	};

	if (strcmp(cmd, "GET") == 0)
	{
		char *key = strtok(NULL, " ");

		char query[10000];
		sprintf(query, "GET %s", key);

		write(sock, query, strlen(query));

		char buf[10000];
		read(sock, buf, 10000);

		printf("%s\n", buf);
	};

	return 0;
}

int launch_interactive_shell(char *host, int port)
{
	// Connect to hache service
	int sock = net_client_connect(host, port);
	glob_sock = sock;

	// Handle signals
	signal(SIGINT, sigint_handler);

	while (1)
	{
		// Receive user input
	    printf("> ");
	    char *args = readline("");
	    
	    int res = execute_shell_command(args, sock);
	    
	    if (res == 1)
	    {
	    	break;
	    }
	};

	return 0;
}

void sigint_handler(int sig)
{
	printf("\n>> Closing socket...\n");
	close(glob_sock);
	exit(0);
}
