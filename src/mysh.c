#include "shell.h"
#include "../include/command.h"

int main(void)
{
    Command command;

    get_command(&command);

    while (!is_exit(&command)) {
        run_command(&command);
        get_command(&command);
    }

    return 0;
}
