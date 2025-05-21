#include "Memory.h"
#include <assert.h>

#include <stdio.h>

int main(int argc, char **argv) {
static_assert(5 > 0);
  int *i = rxalloc(int);
  rxfree(i);  
  i = assign_memory(sizeof(int),ALLOCATED_MEMORY); 
  rxfree(i);  
  long *i1 = rxalloc(long);
  *i = 5;

  return 0;
}
