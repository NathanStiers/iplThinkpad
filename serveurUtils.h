#ifndef _SERVEURUTILS_H_
#define _SERVEURUTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "utils.h"

#define MAX_UTILISATEURS 50
#define KEY 42
#define TAILLEPHYSIQUE 1000

typedef struct{
	int id;
	char nomFichier[256];
	int erreurCompil;
	int nbrExec;
	long dureeExecTotal;
} Programme;

Programme* listeProgramme[TAILLEPHYSIQUE];
int tailleLogique;

int initServeur(int port);

void lireMessageClient(structMessage * msg, int sockfd);

void ecrireMessageClient(structMessage * msg, int sockfd);

void init_shm();

void shmdtCheck();

void detruire_shm();

/**
 * Renvoie l'indice de l'id si présent, -1 sinon
 */
int contains(int id);

#endif