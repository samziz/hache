#ifndef SHELL_H
#define SHELL_H

int launch_interactive_shell();
int execute_shell_command(char *cmd, int sock);
char *parse_shell_command(char *args);

#endif
