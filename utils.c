#include "utils.h"

//*****************************************************************************
// VERIFICATION
//*****************************************************************************

void checkCond(bool cond, char* msg) {
  if (cond) {
    perror(msg);
    exit(EXIT_FAILURE);
  }  
}

void checkNeg(int res, char* msg) {
  checkCond(res < 0, msg);
}

void checkNull(void* res, char* msg) {
  checkCond(res == NULL, msg);
}

//*****************************************************************************
// FICHIERS
//*****************************************************************************
int openConfig(char* path) {
  int fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
  checkNeg(fd, "Error OPEN");
  return fd;
}

void writeCheck(int fd, void* buff, int size) {
  int r = write(fd, buff, size);
  checkCond(r != size, "Error WRITE");
}

int readCheck(int fd, void* buff, int size) {
  int r = read(fd, buff, size);
  checkNeg(r, "Error READ");
  return r;
}

// READ EXACTLY n chars
int nread(int fd, void* buff, int n) {
  char* cbuff = (char*) buff;
  int s = readCheck(fd, cbuff, n);
  int i = s;
  while(s != 0 && i != n) {
    i += s;
    s =  readCheck(fd, cbuff + i, n - i);
  }
  return s;
}

void closeCheck(int fd) {
  int r = close(fd);
  checkNeg(r, "Error CLOSE");
}

//*****************************************************************************
// FORK
//*****************************************************************************

pid_t fork_and_run(void (*handler)()) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");
  
  // child process
  if (childId == 0) {  
    (*handler)();
    exit(EXIT_SUCCESS);
  }
  
  return childId;
}

pid_t fork_and_run_arg(void (*handler)(), void* arg0) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run_arg]");
  
  // child process
  if (childId == 0) {  
    (*handler)(arg0);
    exit(EXIT_SUCCESS);
  }
  
  return childId;
}

pid_t fork_and_run_arg_arg(void (*handler)(), void* arg0, void* arg1) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run_arg_arg]");

  // child process
  if (childId == 0) {  
    (*handler)(arg0, arg1);
    exit(EXIT_SUCCESS);
  }
  
  return childId;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}





