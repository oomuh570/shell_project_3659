CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude

# --- Main shell ---
MYSH_SRC = src/mysh.c
LIB_SRC  = src/shell.c src/command.c src/alloc.c src/strlib.c

MYSH_OBJ = $(MYSH_SRC:.c=.o)
LIB_OBJ  = $(LIB_SRC:.c=.o)

TARGET   = mysh

# --- Tests ---
TEST_SRCS  = $(wildcard tests/test_*.c)
TEST_BINS  = $(TEST_SRCS:.c=)

.PHONY: all clean run tests

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

clean:
	rm -f $(MYSH_OBJ) $(LIB_OBJ) $(TARGET) tests/test_*.o $(TEST_BINS)
