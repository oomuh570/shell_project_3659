#include <stdio.h>
#include "../include/shell.h"
#include "../include/command.h"

int main(void)
{
    Command cmd;

    printf("Testing run_command...\n");

    // ---- Test 1: /bin/echo hello world ----
    command_init(&cmd);
    cmd.argv[0] = "/bin/echo";
    cmd.argv[1] = "hello";
    cmd.argv[2] = "world";
    cmd.argv[3] = NULL;
    cmd.argc = 3;

    printf("\n[TEST 1] Expect: hello world\n");
    run_command(&cmd);

    // ---- Test 2: /bin/ls ----
    command_init(&cmd);
    cmd.argv[0] = "/bin/ls";
    cmd.argv[1] = NULL;
    cmd.argc = 1;

    printf("\n[TEST 2] Expect: directory listing\n");
    run_command(&cmd);

    // ---- Test 3: invalid path ----
    command_init(&cmd);
    cmd.argv[0] = "/bin/does_not_exist";
    cmd.argv[1] = NULL;
    cmd.argc = 1;

    printf("\n[TEST 3] Expect: execve failed message\n");
    run_command(&cmd);

    printf("\nDone.\n");
    return 0;
}
