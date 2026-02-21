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

    if (strcntn("hello", 'h'))
      write(1, "contains\n", 9);

    if (!strcntn("hello", '/'))
      write(1, "not found\n", 10); 
	
    return 0;
}
