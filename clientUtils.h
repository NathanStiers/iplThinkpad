#ifndef _CLIENTUTILS_H_
#define _CLIENTUTILS_H_

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "message.h"

#define TABECOUTE 2
#define SERVER_IP "127.0.0.1"
#define PORT_IP 8080

int sockfd;

int sockfdExec;

int initSocketClient(char ServerIP[16], int Serverport);

void initSocketClientExec(char ServerIP[16], int Serverport);

void ecrireMessageAuServeur(structMessage* msg);

void lireMessageDuServeur(structMessage* msg);

void filsMinuterie(int* delay, int pipefdMinuterie[]);

void filsExecution(int pipefdExec[]);

void terminal(int pipefdExec[], int fdMinuterie, int fdExec);

void afficherMessageCmd();

void connexionServeur();

#endif