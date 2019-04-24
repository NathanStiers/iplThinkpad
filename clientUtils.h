#include <stdlib.h>
#include <stdio.h>

#define SERVER_IP "127.0.0.1"
#define MAX_PSEUDO 256

int initSocketClient(char ServerIP[16], int Serverport);

void ecrireMessageAuServeur();

void lireMessageDuServeur();