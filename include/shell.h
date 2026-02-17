#ifndef SHELL_H
#define SHELL_H

#include "command.h"

void get_command(Command *cmd);
void run_command(const Command *cmd);
int is_exit(const Command *cmd);

#endif
