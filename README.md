# COMP 3659 – Programming Project 1: mysh (Linux Shell)

## Overview

`mysh` is a custom Linux shell developed in C for COMP 3659 – Operating Systems. It provides a command line interface supporting a subset of functionality found in common Linux shells such as bash. The shell runs in interactive mode, repeatedly prompting the user for input and executing commands until the user types `exit`.

---

## Features

### Core Features
- Interactive prompt (`$ `)
- Execute external commands using absolute paths (e.g. `/bin/ls`)
- Command path search via `PATH` environment variable (e.g. type `ls` instead of `/bin/ls`)
- Built-in commands: `exit`, `cd`, `pwd`
- Graceful exit on `exit` command or EOF (Ctrl+D)

### Pipelines
- Two-stage pipelines: `ls | wc -l`
- N-stage pipelines: `ls | wc -l | wc -l | wc -l`

### I/O Redirection
- Output redirection: `ls > out.txt`
- Input redirection: `wc -l < out.txt`
- Combined: `ls | wc -l > count.txt`

### Background Processing
- Run commands in the background using `&`: `sleep 5 &`
- Shell returns prompt immediately without waiting for background job to finish

### Signal Handling
- `Ctrl+C` (SIGINT) kills the foreground process but does NOT kill the shell
- Shell ignores SIGINT; child processes reset SIGINT to default

### Error Handling
- All system calls checked for errors with descriptive messages using `errno` and `strerror`
- Syntax errors reported clearly (e.g. `syntax error: '|' cannot start command`)

---

## Building

```
make
```

This compiles all source files and produces the `mysh` executable.

To clean build artifacts:
```
make clean
```

---

## Running

```
./mysh
```

### Example Session
```
$ /bin/ls
README.md  include  makefile  mysh  src  tests
$ ls | wc -l
6
$ cd /tmp
$ pwd
/tmp
$ cd /
$ ls > out.txt
$ wc -l < out.txt
6
$ sleep 5 &
$ exit
```

---

## Built-in Commands

| Command | Description |
|---------|-------------|
| `exit` | Exits the shell |
| `cd <dir>` | Changes the current directory |
| `pwd` | Prints the current working directory |

---

## Project Structure

```
.
├── makefile
├── README.md
├── include/
│   ├── shell.h       # Shell function declarations
│   ├── job.h         # Job struct and definitions
│   ├── command.h     # Command struct and definitions
│   └── alloc.h       # Memory allocator declarations
├── src/
│   ├── mysh.c        # Main entry point
│   ├── shell.c       # Core shell logic
│   ├── job.c         # Job initialization
│   ├── command.c     # Command initialization
│   ├── alloc.c       # Custom heap allocator
│   └── strlib.c      # String utilities
└── tests/
    ├── test_basic.txt
    ├── test_pipes.txt
    ├── test_pipe_errors.txt
    ├── test_redirection.txt
    ├── test_redirection_errors.txt
    ├── test_background.txt
    ├── test_builtins.txt
    ├── test_memory.txt
    └── run_tests.sh
```

---

## Testing

### Running All Tests
```
make test_input
```

### Running Individual Test Sections
```
make test_basic              # Basic command tests
make test_pipes              # Pipeline tests
make test_pipe_errors        # Pipe syntax error tests
make test_redirection        # I/O redirection tests
make test_redirection_errors # Redirection error tests
make test_background         # Background job tests
make test_builtins           # Built-in command tests
make test_memory             # Memory management tests
```

### Manual Signal Test
Run the shell, type the command below, then press `Ctrl+C`:
```
sleep 10
```
Expected: sleep dies, shell stays alive and returns `$` prompt.

---

## Error Messages

| Error | Cause |
|-------|-------|
| `syntax error: '|' cannot start command` | Pipe at start of command |
| `syntax error: '|' must be followed by command` | Pipe at end or double pipe |
| `syntax error: '&' must be at end` | `&` not at end of command |
| `Expected output file after '>'` | Missing filename after `>` |
| `Expected input file after '<'` | Missing filename after `<` |
| `command not found` | Command not found in PATH |
| `execve failed: <reason>` | Command execution failed |
| `open failed: <reason>` | File open failed |
| `cd failed: <reason>` | Directory change failed |
| `cd: missing argument` | No argument given to cd |
| `Line too long` | Input exceeds 128 characters |

---

## Implementation Notes

- No use of `malloc` or `free` — uses a custom heap allocator (`alloc.c`)
- No use of standard library string functions — custom implementations in `strlib.c`
- All I/O done via `read()` and `write()` system calls
- Pipelines handled by creating a new pipe between each stage
- Background jobs forked and not waited on
- Signal handling via `signal()` — shell ignores SIGINT, children reset to default
- Command path search via `find_path()` which reads the `PATH` environment variable

---

## Limitations

- No command history
- No tab completion
- No shell variables or expansion
- No advanced job control (fg, bg, jobs)
- Command line length limited to 128 characters
- No support for quoting or escape characters

---
