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
int openConfig(char* path, int flag, int mode) {
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





