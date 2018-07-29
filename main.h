#ifndef MAIN_H
#define MAIN_H

#include "db.h"
#include "net.h"

void execute_command(char *cmd);
void launch_local_service();
int main(int argc, char *argv[]);

#endif
