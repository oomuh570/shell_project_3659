CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude

# --- Main shell ---
MYSH_SRC = src/mysh.c
LIB_SRC  = src/shell.c src/job.c src/command.c src/alloc.c src/strlib.c src/signals.c

MYSH_OBJ = $(MYSH_SRC:.c=.o)
LIB_OBJ  = $(LIB_SRC:.c=.o)

TARGET   = mysh

# --- Tests ---
TEST_SRCS  = $(wildcard tests/test_*.c)
TEST_BINS  = $(TEST_SRCS:.c=)

.PHONY: all clean run tests test_input test_basic test_pipes test_pipe_errors test_redirection test_redirection_errors test_background test_builtins test_memory

all: $(TARGET)

$(TARGET): $(MYSH_OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compile any .c into matching .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build ALL tests (each tests/test_x.c -> tests/test_x)
tests: $(TEST_BINS)

# Link each test with the common library objects
tests/test_%: tests/test_%.o $(LIB_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

run: all
	./$(TARGET)

# --- Individual input file tests ---
test_basic: all
	./$(TARGET) < tests/test_basic.txt

test_pipes: all
	./$(TARGET) < tests/test_pipes.txt

test_pipe_errors: all
	./$(TARGET) < tests/test_pipe_errors.txt

test_redirection: all
	./$(TARGET) < tests/test_redirection.txt

test_redirection_errors: all
	./$(TARGET) < tests/test_redirection_errors.txt

test_background: all
	./$(TARGET) < tests/test_background.txt

test_builtins: all
	./$(TARGET) < tests/test_builtins.txt

test_memory: all
	./$(TARGET) < tests/test_memory.txt

# --- Run all input file tests ---
test_input: all
	@echo "=============================="
	@echo " Running Basic Commands"
	@echo "=============================="
	./$(TARGET) < tests/test_basic.txt

	@echo "=============================="
	@echo " Running Pipe Tests"
	@echo "=============================="
	./$(TARGET) < tests/test_pipes.txt

	@echo "=============================="
	@echo " Running Pipe Error Tests"
	@echo "=============================="
	./$(TARGET) < tests/test_pipe_errors.txt

	@echo "=============================="
	@echo " Running Redirection Tests"
	@echo "=============================="
	./$(TARGET) < tests/test_redirection.txt

	@echo "=============================="
	@echo " Running Redirection Errors"
	@echo "=============================="
	./$(TARGET) < tests/test_redirection_errors.txt

	@echo "=============================="
	@echo " Running Background Tests"
	@echo "=============================="
	./$(TARGET) < tests/test_background.txt

	@echo "=============================="
	@echo " Running Builtin Tests"
	@echo "=============================="
	./$(TARGET) < tests/test_builtins.txt

	@echo "=============================="
	@echo " Running Memory Tests"
	@echo "=============================="
	./$(TARGET) < tests/test_memory.txt

	@echo "=============================="
	@echo " All tests complete"
	@echo "=============================="

clean:
	rm -f $(MYSH_OBJ) $(LIB_OBJ) $(TARGET) tests/test_*.o $(TEST_BINS)
