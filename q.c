#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
//#include <sys/mman.h>
#include <sys/wait.h>

int main (){
  pid_t child_pid;
  int i, status, n=3;
  for (i=0;i<3;i++){
    child_pid=fork();
     if (child_pid==0) break;
  }

  printf("(i = %d): PID = %d \t Parent PID = %d\n",
   i, getpid(), getppid());

  fflush(NULL);
  wait(&status);
}
