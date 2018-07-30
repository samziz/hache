#ifndef SHELL_H
#define SHELL_H

int sh_launch_interactive(char *host, int port);
int sh_execute_cmd(char *cmd, int sock);

#endif
