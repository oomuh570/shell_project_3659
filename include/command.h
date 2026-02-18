#ifndef COMMAND_H
#define COMMAND_H

#define MAX_ARGS 32   

typedef struct {
    char *argv[MAX_ARGS + 1];  // array of argument strings, +1 for NULL terminator
    unsigned int argc;         // number of arguments
    int background;           // flag indicating if the command should run in the background
   
} Command;


/*
 * command_init()
 *
 * Resets a Command structure so it can safely
 * represent a new command.
 *
 * Sets argc to 0 and initializes all argv
 * entries to NULL.
 */

void command_init(Command *cmd);

#endif
