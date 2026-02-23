#ifndef SIGNALS_H
#define SIGNALS_H


/* Simple Signal Handling interface for the shell that handles SIGINT
(ctrl-c)
- so that the shell does not exit when ctrl -c is pressed and
- the current input line is interrupted
*/



/* initializes signal handlers*/
void signals_init(void);

/* returns non-zero if SIGINT was recieved */
int  signals_got_sigint(void);

/*clears the SIGINT flag after its handled */
void signals_clear_sigint(void);

#endif
