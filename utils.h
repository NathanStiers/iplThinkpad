#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_LONGUEUR 256

//MESSAGE CODE
#define DEMANDE_CONNEXION 1
#define CONNEXION_REUSSIE 2
#define CONNEXION_ECHOUEE 3

void checkCond(bool cond, char* msg);

void checkNeg(int res, char* msg);

void checkNull(void* res, char* msg);

typedef struct {
	char MessageText[MAX_LONGUEUR];
	int code;
} structMessage;

#endif
