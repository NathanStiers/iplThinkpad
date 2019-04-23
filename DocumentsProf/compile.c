#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Utility functions

void checkCond(bool cond, char* msg) {
  if (cond) {
    perror(msg);
    exit(1);
  }  
}

void checkNeg(int res, char* msg) {
  checkCond(res < 0, msg);
}


pid_t fork_and_run(void (*handler)()) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");
  
  // child process
  if (childId == 0) {  
    (*handler)();
    exit(0);
  }
  
  return childId;
}

long now() {
  struct timeval  tv;
  
  int res = gettimeofday(&tv, NULL);
  checkNeg(res, "Error gettimeofday");
  
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

// Handlers

void handler1() {
  execl("/usr/bin/gcc", "gcc", "-o", "hello", "hello.c", NULL);
  perror("Error execl 1");
}

void handler2() {
  execl("./hello", "hello", NULL);
  perror("Error exec 2");
}

// Main program

int main() {
  printf("**************************\n");
  printf("CRÉATION DU FICHIER res_compile.txt\n");
  printf("**************************\n");
  int fd = open("res_compile.txt", O_CREAT | O_WRONLY| O_TRUNC, 0666);
  checkNeg(fd, "ERROR open");
  
  printf("**************************\n");
  printf("REDIRECTION DE STDERR\n");
  printf("**************************\n");
  int stderr_copy = dup(2);
  checkNeg(stderr_copy, "ERROR dup");
  
  int ret = dup2(fd, 2);
  checkNeg(ret, "ERROR dup2");

  printf("**************************\n");
  printf("COMPILATION DU FICHIER hello.c\n");
  printf("**************************\n");
  fork_and_run(handler1);
  int status;
  wait(&status);
  printf("SI %d != 0, ALORS regarde dans res_compile.txt\n", WEXITSTATUS(status));
  
  printf("**************************\n");
  printf("RÉTABLISSEMENT DE STDERR\n");
  printf("**************************\n");
  ret = dup2(stderr_copy, 2);
  checkNeg(ret, "ERROR dup");
  close(stderr_copy);

  if (WIFEXITED(status) && !WEXITSTATUS(status)) {
    printf("**************************\n");
    printf("EXÉCUTION\n");
    printf("**************************\n");
    long t1 = now();
    fork_and_run(handler2);
    wait(&status);
    long t2 = now();
    printf("SI %d != 0 ALORS execution ok\n", WIFEXITED(status));
    printf("SI exécution ok ALORS les statut de l'exécution = %d\n", WEXITSTATUS(status));
    printf("Le temps d'exécution = %ld\n", t2 - t1);
  }

  return 0;
}
