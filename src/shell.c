
#include <unistd.h>     // read, write, _exit
#include <sys/wait.h>   // waitpid
#include "../include/shell.h"
#include "../include/command.h"
#include "../include/alloc.h"     

#define MAX_LINE 128  

static void print_prompt(void) {
    write(1, "$ ", 2);
}

/* Reads a line into buf (null-terminated).
   Returns:
     >=0 : length of line
     -1  : EOF or read error
     -2  : line too long (and flushed) */
static int read_line(char *buf, int max) {
    int n = 0;
    char c;

    while (1) {
        int r = read(0, &c, 1);
        if (r <= 0) return -1;

        if (c == '\n') {
            buf[n] = '\0';
            return n;
        }

        if (n < max - 1) {
            buf[n++] = c;
        } else {
            /* too long: flush rest of line */
            while (c != '\n') {
                r = read(0, &c, 1);
                if (r <= 0) break;
            }
            buf[0] = '\0';
            return -2;
        }
    }
}

/* Copy line[start..end-1] into heap using alloc() */
static char *copy_token(const char *line, int start, int end) {
    int len = end - start;
    char *tok = alloc(len + 1);      /* +1 for '\0' */
    if (!tok) return 0;

    for (int i = 0; i < len; i++) {
        tok[i] = line[start + i];
    }
    tok[len] = '\0';
    return tok;
}

/* Tokenize by spaces/tabs into cmd->argv/cmd->argc.
   Handles "&" ONLY if it is the last token.
   "&" is NOT stored in argv; it sets cmd->background = 1. */
static void tokenize(const char *line, Command *cmd) {
    int i = 0;

    while (line[i] != '\0') {
        /* skip whitespace */
        while (line[i] == ' ' || line[i] == '\t') i++;
        if (line[i] == '\0') break;

        int start = i;
        while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t') i++;
        int end = i;

        /* Detect token exactly "&" */
        if ((end - start) == 1 && line[start] == '&') {
            /* Only valid if it's the LAST token (ignoring trailing spaces) */
            int j = end;
            while (line[j] == ' ' || line[j] == '\t') j++;

            if (line[j] == '\0') {
                cmd->background = 1;
                return; /* stop tokenizing */
            } else {
                write(2, "& must be at end\n", 16);
                return;
            }
        }

        if (cmd->argc >= MAX_ARGS) {
            write(2, "Too many arguments\n", 19);
            return;
        }

        char *tok = copy_token(line, start, end);
        if (!tok) {
            write(2, "Out of heap memory\n", 19);
            return;
        }

        cmd->argv[cmd->argc++] = tok;
        cmd->argv[cmd->argc] = 0; /* keep NULL-terminated */
    }
}

int is_exit(const Command *cmd) {
    if (cmd->argc != 1) return 0;
    char *s = cmd->argv[0];
    return (s[0]=='e' && s[1]=='x' && s[2]=='i' && s[3]=='t' && s[4]=='\0');
}

void get_command(Command *cmd) {
    char line[MAX_LINE];

    command_init(cmd);
    print_prompt();

    int r = read_line(line, MAX_LINE);
    if (r == -1) {
        /* EOF/error -> exit */
        cmd->argv[0] = "exit";
        cmd->argc = 1;
        cmd->argv[1] = 0;
        return;
    }
    if (r == -2) {
        write(2, "Line too long\n", 14);
        return;
    }

    /* reset heap before tokenizing */
    free_all();

    /* tokenize + fill cmd, may set cmd->background */
    tokenize(line, cmd);
}

void run_command(const Command *cmd) {
    if (cmd->argc == 0) return;

    pid_t pid = fork();

    if (pid < 0) {
        write(2, "fork failed\n", 12);
        return;
    }

    if (pid == 0) {
        /* child */
        extern char **environ;
        execve(cmd->argv[0], cmd->argv, environ);

        /* if execve returns, it failed */
        write(2, "execve failed\n", 14);
        _exit(127);
    }

    /* parent: only wait if NOT background */
    if (!cmd->background) {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            write(2, "waitpid failed\n", 15);
        }
    } else {
        /* simple background: do not wait */
    }
}
