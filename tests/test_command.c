#include <stdio.h>
#include "../include/command.h"

static void print_summary(int passed, int failed) {
    printf("\n====================\n");
    printf("TEST SUMMARY\n");
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    printf("====================\n");
}


int main(void)
{
    Command cmd;
    unsigned int i;

    int passed = 0;
    int failed = 0;

    printf("Running Command tests...\n");

    // ---- Test: command_init ----
    printf("\n[TEST] command_init()\n");
    command_init(&cmd);

    // Check argc
    printf("  - Check argc == 0 ... ");
    if (cmd.argc == 0) {
        printf("PASS\n");
        passed++;
    } else {
        printf("FAIL (argc=%u)\n", cmd.argc);
        failed++;
    }

    // Check all argv are NULL
    printf("  - Check argv[i] == NULL for all i ...\n");
    int all_null = 1;
    for (i = 0; i < MAX_ARGS + 1; i++) {
        if (cmd.argv[i] != NULL) {
            printf("    FAIL: argv[%u] is not NULL (value=%p)\n", i, (void*)cmd.argv[i]);
            all_null = 0;
            failed++;
            break;
        }
    }
    if (all_null) {
        printf("    PASS: all argv entries are NULL\n");
        passed++;
    }

    // Dump first few argv entries for visibility
    printf("\n[INFO] argv dump (first 8 entries):\n");
    for (i = 0; i < 8 && i < (MAX_ARGS + 1); i++) {
        printf("  argv[%u] = %p\n", i, (void*)cmd.argv[i]);
    }

    print_summary(passed, failed);

    // Exit code: 0 if all pass, 1 if any fail
    return (failed == 0) ? 0 : 1;
}
