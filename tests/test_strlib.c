#include <unistd.h>
#include "../include/strlib.h"

int main()
{
    char buf[100];

    if (streq("exit", "exit"))
        write(1, "streq OK\n", 9);

    if (strlen2("hello") == 5)
        write(1, "strlen OK\n", 10);

    strcpy2(buf, "copy works");
    write(1, buf, strlen2(buf));
    write(1, "\n", 1);

    return 0;
}
