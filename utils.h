#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdio.h>

#include <stdbool.h>

void checkCond(bool cond, char* msg);

void checkNeg(int res, char* msg);

void checkNull(void* res, char* msg);

#endif