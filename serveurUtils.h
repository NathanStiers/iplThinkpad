#ifndef _SERVEURUTILS_H_
#define _SERVEURUTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_UTILISATEURS 50

typedef struct{
	int id;
	char nomFichier[256];
	int erreurCompil;
	int nbrExec;
	long dureeExecTotal;
} Programme;

int initServeur(int port);

#endif