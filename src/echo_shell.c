#include <unistd.h>

#define MAX 128 //max length of input

int main() {
  char buf[MAX];
  int n;

  while (1) {

    n = 0; //reset buffer index

    //this is the input loop, reads one line, char by char
    while (1) {
      char c;

      if (read(0, &c, 1) <= 0)
	return 0;

      if (c == '\n') {
	buf[n] = '\0';
	break;
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
      break;
	}

    //echo input back to user
    write(1, buf, n);
    write(1, "\n", 1);
  }

  return 0;
}
