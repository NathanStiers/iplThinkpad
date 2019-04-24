#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

#define SERVER_IP "127.0.0.1"
#define MAX_PSEUDO 256 // PAS UTILE JE PENSE

void initSocketClient(char ServerIP[16], int Serverport);

void ecrireMessageAuServeur(structMessage* msg);

void lireMessageDuServeur(structMessage* msg);