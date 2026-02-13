#include <unistd.h>
#include "../include/alloc.h"

int main()
{
  char *a = alloc(50);
  if(a == NULL){
    write(1, "Allocation 1 failed\n", 20);
    return 1; //exit status is failure
  }

  for(int i = 0; i < 50; i++)
    a[i] = 'A';

  char *b = alloc(100);
  if(b == NULL){
    write(1, "Allocation 2 failed\n", 20);
    return 1; //exit status is failure
  }

  for(int i = 0; i < 100; i++)
    b[i] = 'B';

  write(1, "char at heap[0]: ", 17);
  write(1, a, 1);
  write(1, "\n", 1);

  write(1, "char at heap[49]: ", 18);
  write(1, a + 49, 1);
  write(1, "\n", 1);
  
  write(1, "char at heap[50]: ", 18);
  write(1, a + 50, 1);
  write(1, "\n", 1);
  
  free_all();
  write(1, "heap reset\n", 11);

  char *c = alloc(50);
  for(int i = 0; i < 50; i++)
    c[i] = 'C';

  write(1, "char at heap[0]: ", 17);
  write(1, c, 1);
  write(1, "\n", 1);

  write(1, "char at heap[50]: ", 18);
  write(1, c + 50, 1);
  write(1, "\n", 1);

  write(1, "tests complete\n", 15);
}
  
  

 

  

  
