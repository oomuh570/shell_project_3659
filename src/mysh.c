/*

File Name: mysh.c
Author(s): Slater Dennington, Ochihai Omuha, Salman Tajammal, Dan Meulendyk
Creation Date: Feb 14th, 2026
Last Modified Date: Feb 14th, 2026
Purpose: Executes the shell

*/

#include "../include/shell.h"
#include "../include/signals.h"
/*

FUNCTION: main
PURPOSE: Runs the shell
INPUT: none
OUTPUT: none

*/

int main(void)
{

	/* shell ignores SIGINT so Ctrl+C does not kill it */
    signals_init();

	Job job;
	
	get_job(&job);
	
	while (!is_exit(&job)) {
		run_job(&job);
		get_job(&job);
	}
	
	return 0;

// Single command version
/*    Command command;

    get_command(&command);

    while (!is_exit(&command)) {
        run_command(&command);
        get_command(&command);
    }

    return 0;*/
}
