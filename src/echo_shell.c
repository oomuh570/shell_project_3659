#include <unistd.h>

#define MAX 128 //max length of input

int main() {
  char buf[MAX];
  int n;
  int exit_flag = 0; //made an exit flag, could use bools but wasn't working atm in emacs
  int input_flag = 0; //this flag is for when input is done

  while (exit_flag != 0) {

    n = 0; //reset buffer index
    exit_flag = 0;

    //this is the input loop, reads one line, char by char
    while (input_flag != 0) {
      char c;
      input_flag = 0;
      
      if (read(0, &c, 1) <= 0)
	return 0;

      if (c == '\n') {
	buf[n] = '\0';
	input_flag = 1;
      }

      if (n < MAX - 1)
	buf[n++] = c;
    }

    // checks if user typed "exit"
    if (buf[0] == 'e' &&
	buf[1] == 'x' &&
	buf[2] == 'i' &&
	buf[3] == 't' &&
	buf[4] == '\0') {
      exit_flag = 1;
	}

    //echo input back to user
    write(1, buf, n);
    write(1, "\n", 1);
  }

  return 0;
}
