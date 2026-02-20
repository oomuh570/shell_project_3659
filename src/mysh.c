#include "../include/shell.h"

int main(void)
{
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