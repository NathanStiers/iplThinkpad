#ifndef _CLIENTUTILS_H_
#define _CLIENTUTILS_H_

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

#define SERVER_IP "127.0.0.1"

void initSocketClient(char ServerIP[16], int Serverport);

void ecrireMessageAuServeur(structMessage* msg);

void lireMessageDuServeur(structMessage* msg);

void filsMinuterie(int* delay, int pipefdMinuterie[]);

void filsExecution(int pipefdExec[]);

void terminal();

void afficherMessageCmd();

#endif