/*

File Name: shell.c
Author(s): Slater Dennington, Ochihai Omuha, Salman Tajammal, Dan Meulendyk
Creation Date: Feb 14th, 2026
Last Modified Date: Feb 22nd, 2026
Purpose: Contains all the functions for implementing the shell

*/

#include <fcntl.h> 		// open() flags O_RDONLY, O_WRONLY, etc.
#include <unistd.h>     // read, write, _exit
#include <sys/wait.h>   // waitpid
#include <errno.h>      // errno
#include <string.h>     // strerror
#include <signal.h>     // signal
#include <sys/stat.h>
#include "../include/shell.h"
#include "../include/command.h"
#include "../include/alloc.h"
#include "../include/job.h"
#include "../include/strlib.h"
#include "../include/signals.h"
#define MAX_LINE 128
#define MAX_PATH 1024

/* Helper macro to write an error message with strerror 
#define ERR_SYS(msg) do { \
    write(2, msg ": ", sizeof(msg)+1); \
    write(2, strerror(errno), strlen(strerror(errno))); \
    write(2, "\n", 1); \
} while(0)

*/
static void err_sys(const char *msg)
{
    const char *e = strerror(errno);
    write(2, msg, strlen2(msg));
    write(2, ": ", 2);
    write(2, e, strlen2(e));
    write(2, "\n", 1);
}

static void print_prompt(void) {
    write(1, "$ ", 2);
}

static int read_line(char *buf, int max) {
    int n = 0;
    char c;

    while (1) {
        int r = read(0, &c, 1);
        if (r < 0) {
		if (errno == EINTR) {
	return -3; //interuppted by CTRL:-C
	}
            err_sys("read failed");
            return -1;
        }
        if (r == 0) return -1;

        if (c == '\n') {
            buf[n] = '\0';
            return n;
        }

        if (n < max - 1) {
            buf[n++] = c;
        } else {
            while (c != '\n') {
                r = read(0, &c, 1);
                if (r <= 0) break;
            }
            buf[0] = '\0';
            return -2;
        }
    }
}

static char *copy_token(const char *line, int start, int end) {
    int len = end - start;
    char *tok = alloc(len + 1);
    if (!tok) return 0;

    for (int i = 0; i < len; i++) {
        tok[i] = line[start + i];
    }
    tok[len] = '\0';
    return tok;
}


/*

FUNCTION: get_next_token
PURPOSE: Retrieves the next token in the command line
INPUT: *line - command line
       *i - position counter
OUTPUT: copy_token - a copy of the token found

*/

static char *get_next_token(const char *line, int *i) {
    int start;
    int end;

    while (line[*i] == ' ' || line[*i] == '\t') {
        (*i)++;
    }

    if (line[*i] == '\0' || line[*i] == '|' || line[*i] == '<' || line[*i] == '>' || line[*i] == '&') {
        return 0; 
    }

    start = *i;
    while (line[*i] != '\0' && line[*i] != ' ' && line[*i] != '\t' && 
           line[*i] != '<' && line[*i] != '>' && line[*i] != '|' && line[*i] != '&') {
        (*i)++;
    }
    end = *i;

    return copy_token(line, start, end);
}

/*

FUNCTION: tokenize
PURPOSE: Creates tokens of each argument in the command line by spaces/tabs into cmd struct.
         Handles "&" ONLY if it is the last token.
         "&" is not stored in argv; sets cmd->background to 1.
INPUT: *line - command line
       *cmd - command being passed
OUTPUT: none

*/

static void tokenize(const char *line, Command *cmd) {
    int i = 0;

    while (line[i] != '\0') {
        while (line[i] == ' ' || line[i] == '\t') i++;
        if (line[i] == '\0') break;

        int start = i;
        while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t') i++;
        int end = i;

        if ((end - start) == 1 && line[start] == '&') {
            int j = end;
            while (line[j] == ' ' || line[j] == '\t') j++;

            if (line[j] == '\0') {
                cmd->background = 1;
                return;
            } else {
                write(2, "& must be at end\n", 17);
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
        cmd->argv[cmd->argc] = 0;
    }
}

/*

FUNCTION: tokenize_job
PURPOSE: Tokenizes jobs with pipelines in mind
INPUT: *line - command line
       *job - job to be tokenized
OUTPUT: none

*/

static void tokenize_job(const char *line, Job *job) {
    int i = 0;
    int start;
    int end;
    unsigned int current_stage = 0;
    char *tok;
	
    while (line[i] != '\0') {
        while (line[i] == ' ' || line[i] == '\t') i++;
        if (line[i] == '\0') break;
		
        start = i;
        while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t' && 
               line[i] != '<' && line[i] != '>' && line[i] != '|' && line[i] != '&') {
            i++;
        }
        end = i;

        if (start == end) end = ++i;
		
        tok = copy_token(line, start, end);
		
        if (tok[0] == '|') {
            if (job->pipeline[current_stage].argc == 0) {
                write(2, "syntax error: '|' cannot start command\n", 40);
                job->num_stages = 0;
                return;
            } 
            else if (current_stage >= MAX_PIPELINE_LEN - 1) {
                write(2, "syntax error: too many pipeline stages\n", 40);
                job->num_stages = 0;
                return;
            } 

            current_stage++;
            job->num_stages = current_stage + 1;

            while (line[i] == ' ' || line[i] == '\t') i++;
            if (line[i] == '\0' || line[i] == '|' || line[i] == '<' || line[i] == '>' || line[i] == '&') {
                write(2, "syntax error: '|' must be followed by command\n", 47);
                job->num_stages = 0;
                return;
            }
        }
        else if (tok[0] == '<') {
            job->infile_path = get_next_token(line, &i);
            if (!job->infile_path) {
                write(2, "Expected input file after '<'\n", 30);
                return;
            }
        } 
        else if (tok[0] == '>') {
            job->outfile_path = get_next_token(line, &i);
            if (!job->outfile_path) {
                write(2, "Expected output file after '>'\n", 31);
                job->num_stages = 0;
                return;
            }
        } 
        else if (tok[0] == '&') {
            job->background = 1;

            while (line[i] == ' ' || line[i] == '\t') i++;

            if (line[i] != '\0') {
                write(2, "syntax error: '&' must be at end\n", 33);
                job->num_stages = 0;
                job->background = 0;
            }
            return;
        } 
        else {
            Command *cmd = &job->pipeline[current_stage];
            if (cmd->argc < MAX_ARGS) {
                cmd->argv[cmd->argc++] = tok;
                cmd->argv[cmd->argc] = 0;
            }
            if (job->num_stages <= current_stage) job->num_stages = current_stage + 1;
        }
    }
}

/*

FUNCTION: is_exit
PURPOSE: Checks if exit has been written on the command line
INPUT: *job - Job being checked
OUTPUT s = "exit"

*/

int is_exit(Job *job) {
    if (job->num_stages != 1) return 0;
    Command *cmd = &job->pipeline[0];
	if (cmd->argc != 1) return 0;
    char *s = cmd->argv[0];
    return (s[0]=='e' && s[1]=='x' && s[2]=='i' && s[3]=='t' && s[4]=='\0');
}

//SHOULD THIS BE DELETED?
/*
int is_exit(const Command *cmd) {
    if (cmd->argc != 1) return 0;
    char *s = cmd->argv[0];
    return (s[0]=='e' && s[1]=='x' && s[2]=='i' && s[3]=='t' && s[4]=='\0');
}

*/

/*

FUNCTION: get_command
PURPOSE: Getter funciton for finding a command with error checking
INPUT: cmd - command to find
OUTPUT: none

*/

void get_command(Command *cmd) {
    char line[MAX_LINE];

    command_init(cmd);
    print_prompt();

    int r = read_line(line, MAX_LINE);
    if (r == -1) {
        cmd->argv[0] = "exit";
        cmd->argc = 1;
        cmd->argv[1] = 0;
        return;
    }
    if (r == -2) {
        write(2, "Line too long\n", 14);
        return;
    }

    free_all();
    tokenize(line, cmd);
}

/*

FUNCTION: run_command
PURPOSE: runs commands and creates children using fork and execve
INPUT: cmd - command to be run
OUTPUT: none

*/

void run_command(const Command *cmd) {
    if (cmd->argc == 0) return;

    pid_t pid = fork();

    if (pid < 0) {
        err_sys("fork failed");
        return;
    }

    if (pid == 0) {
        extern char **environ;
        execve(cmd->argv[0], cmd->argv, environ);
        err_sys("execve failed");
        _exit(127);
    }

    if (!cmd->background) {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            err_sys("waitpid failed");
        }
    }
}

/*

FUNCTION: find_path
PURPOSE: Creates the path that is written before command for ease of access in the VM
INPUT: cmd - command that is being altered
OUTPUT: String containing full command path for execution 
 
*/

char *find_path(const char *cmd){
  extern char **environ;

  if (strcntn(cmd, '/')){
    return (char *)cmd;
  }

  char *path_env = 0;

  for (int i = 0; environ[i] != 0; i++) {

    //checking if it starts with "PATH="
    char *s = environ[i];

    if (s[0]=='P' && s[1]=='A' && s[2]=='T' &&
	s[3]=='H' && s[4]== '=') {

      path_env = s + 5; //skip "PATH="
      break;
    }

  }

  if(!path_env)
    path_env = "/bin:/usr/bin";

  static char path[MAX_PATH];

  int i =0;
  int start = 0;

  while (1) {

    while(path_env[i] != ':' && path_env[i] != '\0')
      i++;

    int dir_len = i - start;
    int j = 0;

    for (int k = 0; k < dir_len; k++)
      path[j++] = path_env[start + k];

    path[j++] = '/';

    for (int k = 0; cmd[k] != '\0'; k++)
      path[j++] = cmd[k];

    path[j] = '\0';

    struct stat sb;
    if (stat(path, &sb) == 0)
      return path;

    if(path_env[i] == '\0')
      break;

    i++;
    start = i;
  }
  return 0;
}

    	  
//note this is overbuilt and can handle more than 2 commands in an argument

/* 

FUNCTION: run_job
PURPOSE: Runs the job being passed while creating children and duplication using fork() and dup2().
         Executes job using execve(). Handles built-in commands cd and pwd.
INPUT: *job - Job to be processed
OUTPUT: none
         
*/
void run_job(Job *job) {
	if (job->num_stages == 0) return;

    /* handle built-in cd command */
    if (job->num_stages == 1) {
        Command *cmd = &job->pipeline[0];
        if (cmd->argc >= 1 &&
            cmd->argv[0][0] == 'c' &&
            cmd->argv[0][1] == 'd' &&
            cmd->argv[0][2] == '\0') {

            if (cmd->argc < 2) {
                write(2, "cd: missing argument\n", 21);
                return;
            }
            if (chdir(cmd->argv[1]) < 0) {
                err_sys("cd failed");
            }
            return;
        }
    }

    /* handle built-in pwd command */
    if (job->num_stages == 1) {
        Command *cmd = &job->pipeline[0];
        if (cmd->argc >= 1 &&
            cmd->argv[0][0] == 'p' &&
            cmd->argv[0][1] == 'w' &&
            cmd->argv[0][2] == 'd' &&
            cmd->argv[0][3] == '\0') {

            char buf[MAX_PATH];
            if (getcwd(buf, sizeof(buf)) == NULL) {
                err_sys("pwd failed");
            } else {
                write(1, buf, strlen(buf));
                write(1, "\n", 1);
            }
            return;
        }
    }

    int pipefds[2]; // Used if num_stages > 1
    int prev_pipe_read = -1;
    pid_t pids[MAX_PIPELINE_LEN];
    
    for (unsigned int i = 0; i < job->num_stages; i++) {
        if (i < job->num_stages - 1) {
            if (pipe(pipefds) < 0) {
                err_sys("pipe failed");
                return;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            err_sys("fork failed");
            return;
        }

        if (pid == 0) {
            /* reset SIGINT so child can be killed by Ctrl+C */
            signal(SIGINT, SIG_DFL);

            // Input handling
            if (i == 0 && job->infile_path) {
                int fd = open(job->infile_path, O_RDONLY);
                if (fd < 0) { err_sys("open failed"); _exit(1); }
                if (dup2(fd, 0) < 0) { err_sys("dup2 failed"); _exit(1); }
                if (close(fd) < 0) err_sys("close failed");
            } else if (i > 0) {
                if (dup2(prev_pipe_read, 0) < 0) { err_sys("dup2 failed"); _exit(1); }
                close(prev_pipe_read);
            }

            // Output handling
            if (i == job->num_stages - 1 && job->outfile_path) {
                int fd = open(job->outfile_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { err_sys("open failed"); _exit(1); }
                if (dup2(fd, 1) < 0) { err_sys("dup2 failed"); _exit(1); }
                if (close(fd) < 0) err_sys("close failed");
            } else if (i < job->num_stages - 1) {
                if (dup2(pipefds[1], 1) < 0) { err_sys("dup2 failed"); _exit(1); }
                close(pipefds[1]);
                close(pipefds[0]);
            }

            extern char **environ;
	    char *path = find_path(job->pipeline[i].argv[0]);

	    if(!path) {
	      err_sys("execve failed");
	      _exit(127);
	    }

            execve(path, job->pipeline[i].argv, environ);
            err_sys("execve failed");
            _exit(127);
        }

        pids[i] = pid;
	
        if (i > 0) close(prev_pipe_read);
        if (i < job->num_stages - 1) {
            close(pipefds[1]);
            prev_pipe_read = pipefds[0];
        }
    }

    if (!job->background) {
        for (unsigned int i = 0; i < job->num_stages; i++) {
            if (waitpid(pids[i], NULL, 0) < 0) {
                err_sys("waitpid failed");
            }
        }
    }
}

/*

FUNCTION: get_job
PURPOSE: Getter function to find the job to be processed
INPUT: job - Job being processed
OUTPUT: none

*/

void get_job(Job *job) {
    char line[MAX_LINE];

    job_init(job);
    print_prompt();

    int r = read_line(line, MAX_LINE);
    if (r == -1) {
        job->pipeline[0].argv[0] = "exit";
        job->pipeline[0].argc = 1;
        job->num_stages = 1;
        return;
    }
    if (r == -2) {
        write(2, "Line too long\n", 14);
        return;
    }
    if (r == -3) {
	signals_clear_sigint();
	return;
    }

    free_all();
    tokenize_job(line, job);
}

