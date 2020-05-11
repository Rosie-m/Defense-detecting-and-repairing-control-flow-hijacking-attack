#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef STR_SIZE
#define STR_SIZE 16
#endif
void foo1()
{
  printf("in foo1");
  exit(0);
}

void foo2()
{
  printf("in foo2");
  exit(0);
}

int bof(char *str)
{
  void (*myfunc)(void) = &foo1;
  char buffer[8];
  printf("%p\n", myfunc);
  strcpy(buffer, str);
  printf("%p\n", myfunc);
  printf("%p\n", foo2);

  (*myfunc)();
  return 1;
}

int main(int argc, char **argv)
{
  char str[16] = "                ";
  FILE * badfile;
  char dummy[16];
  memset(dummy, 0, 16);
  badfile = fopen("fun_ptr_overflow", "r");
  fread(str, sizeof(char), 16, badfile);
  bof(str);
  printf("Returned Properly\n");
  return 1;
}

