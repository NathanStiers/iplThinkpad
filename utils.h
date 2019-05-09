#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void checkCond(bool cond, char* msg);

void checkNeg(int res, char* msg);

void checkNull(void* res, char* msg);

int openConfig(char* path);

void writeCheck(int fd, void* buff, int size);

int readCheck(int fd, void* buff, int size);

int nread(int fd, void* buff, int n);

void closeCheck(int fd);

pid_t fork_and_run(void (*handler)());

pid_t fork_and_run_arg(void (*handler)(), void* arg0);

pid_t fork_and_run_arg_arg(void (*handler)(), void* arg0, void* arg1);

char* itoa(int i, char b[]);

#endif
