#ifndef SHELL_H
#define SHELL_H


/*** ERRORS ***/

#define E_NO_CMD -1
#define E_BAD_CMD -2
#define E_BAD_OPTS -3


/*** INTERFACE FUNCTIONS ***/

int sh_cmd_handler(int argc, char **argv);


#endif
