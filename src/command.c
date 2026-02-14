#include "../include/command.h"

void command_init(Command *cmd) {
    unsigned int i;
    cmd->argc = 0;
    for (i = 0; i < MAX_ARGS + 1; i++) {
        cmd->argv[i] = 0; /* NULL */
    }
}