#include "../include/alloc.h"

//Heap size (change if running into problems with alloc)
#define HEAP_SZ 10000

//Static so that our heap cannot be changed by other modules
static char heap[HEAP_SZ];
static char *freep = heap;

/*

Function name: alloc
Function purpose: Dynamically allocates memory without using library functions, 
returns NULL pointer if not enough space available in heap
Input: size - amount of bytes to be allocated
Output: start_ptr - pointer to the position in the heap before freep was adjusted

*/

char *alloc(int size)
{
  
  char *start_ptr = freep;
  
  //if not enough available space
  if (start_ptr + size > heap + HEAP_SZ)
    return NULL;

  //move the free pointer by size then return start of chunk
  freep += size;
  return start_ptr;
}

/*

Function name: free_all
Function purpose: Resets freep back to the start of the heap
Input: none
Output: none

*/

void free_all()
{
  //moves pointer to the start of heap
  freep = heap;
}
