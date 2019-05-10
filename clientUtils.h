#ifndef _CLIENTUTILS_H_
#define _CLIENTUTILS_H_

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "message.h"

#define TABECOUTE 2

int sockfd;

int sockfdExec;

int initSocketClient(char ServerIP[16], int Serverport);

void ecrireMessageAuServeur(structMessage* msg);

void lireMessageDuServeur(structMessage* msg);

void connexionServeur(int sockfd, char serverIp[], int port);

#endif