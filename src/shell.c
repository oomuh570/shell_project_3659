#include <fcntl.h> 		// open() flags O_RDONLY, O_WRONLY, etc.
#include <unistd.h>     // read, write, _exit
#include <sys/wait.h>   // waitpid
#include "../include/shell.h"
#include "../include/command.h"
#include "../include/alloc.h"
#include "../include/job.h"     

#define MAX_LINE 128  

/*

FUNCTION: print_prompt
PURPOSE: Writes the char '$' a the beginning of the shell command line
INPUT: none
OUTPUT: none

*/

static void print_prompt(void) {
    write(1, "$ ", 2);
}

/*

FUNCTION: read_line
PURPOSE: Reads the input on the command line into buf, outputs
         errors if read error or line too long
INPUT: char *buf - command line buffer
       int max - max size of buffer
OUTPUT:  0 - length of line
        -1 - read() fails or EOF
        -2 - input line too long

*/

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

/*

FUNCTION: copy_token
PURPOSE: to copy the token into head using the alloc function
INPUT: *line - command line
       start - beginning of line
       end - end of line
OUTPUT: 0 - if there is not enough space
        tok - tokenized array

*/

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



static char *get_next_token(const char *line, int *i) {
    int start;
	int end;
	//
    while (line[*i] == ' ' || line[*i] == '\t') {
        (*i)++;
    }

    if (line[*i] == '\0' || line[*i] == '|' || line[*i] == '<' || line[*i] == '>') {
        //
        return 0; 
    }

    start = *i;
    //
    while (line[*i] != '\0' && line[*i] != ' ' && line[*i] != '\t' && 
           line[*i] != '<' && line[*i] != '>' && line[*i] != '|') {
        (*i)++;
    }
    end = *i;

    return copy_token(line, start, end);
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

static void tokenize_job(const char *line, Job *job) {
	int i = 0;
	int start;
	int end;
	unsigned int current_stage = 0;
	char *tok;
	
	 while (line[i] != '\0') {
        /* skip whitespace */
        while (line[i] == ' ' || line[i] == '\t') i++;
        if (line[i] == '\0') break;
		
		//
        start = i;
        while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t' && 
               line[i] != '<' && line[i] != '>' && line[i] != '|' && line[i] != '&') {
            i++;
        }
        end = i;

        //
        if (start == end) end = ++i;
		
		tok = copy_token(line, start, end);
		
		if (tok[0] == '|') {
            if (current_stage < MAX_PIPELINE_LEN - 1) {
                current_stage++;
                job->num_stages = current_stage + 1;
            }
        } 
		else if (tok[0] == '<') {
            // infile
            job->infile_path = get_next_token(line, &i); 
        } 
		else if (tok[0] == '>') {
            // outfile
            job->outfile_path = get_next_token(line, &i);
        } 
		else if (tok[0] == '&') {
            job->background = 1;
        } 
		else {
            // 
            Command *cmd = &job->pipeline[current_stage];
            if (cmd->argc < MAX_ARGS) {
                cmd->argv[cmd->argc++] = tok;
                cmd->argv[cmd->argc] = 0;
            }
            if (job->num_stages <= current_stage) job->num_stages = current_stage + 1;
        }
	 }
}

int is_exit(Job *job) {
    if (job->num_stages != 1) return 0;
    Command *cmd = &job->pipeline[0];
	if (cmd->argc != 1) return 0;
    char *s = cmd->argv[0];
    return (s[0]=='e' && s[1]=='x' && s[2]=='i' && s[3]=='t' && s[4]=='\0');
}

/*
int is_exit(const Command *cmd) {
    if (cmd->argc != 1) return 0;
    char *s = cmd->argv[0];
    return (s[0]=='e' && s[1]=='x' && s[2]=='i' && s[3]=='t' && s[4]=='\0');
}*/

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

//note this is overbuilt and can handle more than 2 commands in an argument
void run_job(Job *job) {
	if (job->num_stages == 0) return;

    int pipefds[2]; // Used if num_stages > 1
    int prev_pipe_read = -1;
    pid_t pids[MAX_PIPELINE_LEN];
    
    for (unsigned int i = 0; i < job->num_stages; i++) {
        // make pipe
        if (i < job->num_stages - 1) {
            if (pipe(pipefds) < 0) {
                write(2, "pipe failed\n", 12);
                return;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            write(2, "fork failed\n", 12);
            return;
        }

        if (pid == 0) {
            // Input handling
            if (i == 0 && job->infile_path) {
                int fd = open(job->infile_path, O_RDONLY);
                if (fd < 0) { write(2, "file open failed\n", 17); _exit(1); }
                dup2(fd, 0); // 
                close(fd);
            } else if (i > 0) {
                // get input
                dup2(prev_pipe_read, 0);
                close(prev_pipe_read);
            }

            // Output handling
            if (i == job->num_stages - 1 && job->outfile_path) {
                int fd = open(job->outfile_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { write(2, "file open failed\n", 17); _exit(1); }
                dup2(fd, 1); //
                close(fd);
            } else if (i < job->num_stages - 1) {
                // to pipe
                dup2(pipefds[1], 1);
                close(pipefds[1]);
                close(pipefds[0]); //
            }
			extern char **environ;
            execve(job->pipeline[i].argv[0], job->pipeline[i].argv, environ);
            write(2, "execve failed\n", 14);
            _exit(127);
        }

	pids[i] = pid; //adding pid to array of pids
	
        if (i > 0) close(prev_pipe_read);
        if (i < job->num_stages - 1) {
            close(pipefds[1]); // 
            prev_pipe_read = pipefds[0]; // 
        }

    }
    if(!job->background) {
      for(unsigned int i = 0; i < job->num_stages; i++) {
	if (waitpid(pids[i], NULL, 0) < 0) {
	  write(1, "waitpid error", 13);
	}
      }
    }
}

void get_job(Job *job) {
	char line[MAX_LINE];

    job_init(job); //
    print_prompt();

    int r = read_line(line, MAX_LINE);
    if (r == -1) {
        //
        job->pipeline[0].argv[0] = "exit";
        job->pipeline[0].argc = 1;
        job->num_stages = 1;
        return;
    }
    if (r == -2) {
        write(2, "Line too long\n", 14);
        return;
    }

    //
    free_all();

    //
    tokenize_job(line, job);
}
