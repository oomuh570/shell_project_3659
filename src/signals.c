#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <unistd.h>
#include "../include/signals.h"


/*flag set when SIGINT is recieved */
static volatile sig_atomic_t got_sigint = 0;


/* SIGINT handler, sets the flag instead of terminating the shell */
static void on_sigint(int signum)
{
    (void)signum;
    got_sigint = 1;
    write(1, "\n", 1);
}


/* installs the SIGINT handler */
void signals_init(void)
{
    struct sigaction sa;
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, 0);
}

/* checks whether SIGINT was recieved */
int signals_got_sigint(void)
{
    return got_sigint != 0;
}

void signals_clear_sigint(void)
{





    got_sigint = 0;
}

