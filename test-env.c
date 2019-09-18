#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char ** environ;
int main()
{
  char ** env = environ;
  //int i = 0;
  printf("LANG %s \n",getenv("LANG"));
  printf("USER %s \n",getenv("USER"));
  return 0;
}
