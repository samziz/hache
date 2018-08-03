#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include "net.h"

/*** GLOBAL ***/

// Store socket variable globally in case of sigint 
int glob_sock;

struct shell_opts {
	char *host;
	int port;
};


/*** FORWARD DECLARATIONS ***/

void sigint_handler(int sig);
int sh_get_opts(int argc, char **argv, struct shell_opts *opts);
int sh_launch_interactive(char *host, int port);


/*** CLI COMMAND HANDLER ***/

int sh_cmd_handler(int argc, char **argv)
{	    
	if (argc < 3) {
        printf(">> Hache shell module requires a command\n");
        return E_NO_CMD;
    }

    char *mod_cmd = argv[2];
    struct shell_opts opts = {DEFAULT_HOST, DEFAULT_PORT};

    if (sh_get_opts(argc, argv, &opts) != 0) {
        return E_BAD_OPTS;
    }
    
    if (!strcmp(mod_cmd, "start")) {
        sh_launch_interactive(opts.host, opts.port);
        return 0;
    }

    else {
    	printf(">> Command '%s' not valid for Hache shell module\n", mod_cmd);
    	return E_BAD_CMD;
    }
}


/*** INTERNAL FUNCTIONS ***/

int sh_get_opts(int argc, char **argv, struct shell_opts *opts)
{
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp("--host", argv[i])) {
			char *host = argv[i+1];
			opts->host = host;
		}

		else if (!strcmp("--port", argv[i])) {
			char *port = argv[i+1];
			opts->port = atoi(port);
		}
	};

    return 0;
}

int sh_execute_cmd(char *args, int sock)
{
	char *cmd = strtok(args, " ");

	if (!strcmp(cmd, "EXIT")) {
		return 1;
	};

	if (!strcmp(cmd, "SET")) {
		char *key = strtok(NULL, " ");
		char *value = strtok(NULL, " ");
		char query[10000];
		sprintf(query, "SET %s %s", key, value);

		write(sock, query, strlen(query));

		char buf[10000];
		read(sock, buf, 10000);

		printf("%s\n", buf);
	};

	if (!strcmp(cmd, "GET")) {
		char *key = strtok(NULL, " ");

		char query[10000];
		sprintf(query, "GET %s", key);

		write(sock, query, strlen(query));

		char buf[10000];
		read(sock, buf, 10000);
	};

	return 0;
}

int sh_launch_interactive(char *host, int port)
{
	// Connect to hache service
	int sock = net_client_connect(host, port);
	glob_sock = sock;

	// Handle signals
	signal(SIGINT, sigint_handler);

	while (1) {
		// Receive user input
	    printf("> ");
	    char *args = readline("");
	    
	    int res = sh_execute_cmd(args, sock);
	    
	    if (res == 1) {
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
