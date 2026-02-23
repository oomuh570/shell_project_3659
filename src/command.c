/*

File Name: command.c
Author(s): Slater Dennington, Ochihai Omuha, Salman Tajammal, Dan Meulendyk
Creation Date: Feb 14th, 2026
Last Modified Date: Feb 14th, 2026
Purpose: Contains the command intitaion function
 
*/
#include "../include/command.h"


/*

FUNCTION: command_init
PURPOSE: intitiates the structure for commands
INPUT: cmd - command to be make
OUTPUT: none

*/
void command_init(Command *cmd) {
    unsigned int i;

    cmd->argc = 0;
    cmd->background = 0;

    for (i = 0; i < MAX_ARGS + 1; i++) {
        cmd->argv[i] = 0; /* NULL */
    }
}
