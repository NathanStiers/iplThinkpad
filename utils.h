#include <stdlib.h>
#include <stdio.h>

#define MAX_LONGUEUR 256

void checkNeg();

typedef struct {
	char MessageText[MAX_LONGUEUR];
	int code;
} structMessage;