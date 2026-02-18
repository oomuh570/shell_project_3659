#include <stdio.h>
#include "../include/shell.h"
#include "../include/command.h"

int main(void)
{
    Command cmd;

    printf("Type a command (example: /bin/echo hello world)\n");
    get_command(&cmd);

    printf("\n--- get_command results ---\n");
    printf("argc = %u\n", cmd.argc);

    for (unsigned int i = 0; i < cmd.argc; i++) {
        printf("argv[%u] = \"%s\"\n", i, cmd.argv[i]);
    }

    if (cmd.argv[cmd.argc] == NULL) {
        printf("argv[%u] = NULL (good)\n", cmd.argc);
    } else {
        printf("argv[%u] is NOT NULL (bad)\n", cmd.argc);
    }

    return 0;
}
