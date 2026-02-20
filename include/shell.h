#ifndef SHELL_H
#define SHELL_H

#include "command.h"
#include "job.h"

void get_command(Command *cmd);
void run_command(const Command *cmd);
void run_job(Job *job);
void get_job(Job *job);
int is_exit(Job *job);
//int is_exit(const Command *cmd);

#endif
