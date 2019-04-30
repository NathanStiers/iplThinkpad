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
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include "utils.h"

#define MAX_UTILISATEURS 50
#define KEY_SHM 42
#define KEY_SEM 4242
#define TAILLEPHYSIQUE 1000

typedef struct{
	int id;
	char nomFichier[256];
	int erreurCompil;
	int nbrExec;
	long dureeExecTotal;
} Programme;

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

Programme* listeProgramme[TAILLEPHYSIQUE];

int tailleLogique = 0;

int initServeur(int port);

void lireMessageClient(structMessage * msg, int sockfd);

void ecrireMessageClient(structMessage * msg, int sockfd);

void init_shm();

void shmdtCheck();

void detruire_shm();

void init_sem(int val);

void add_sem(int val);

void down();

void up();

void del_sem();

#endif