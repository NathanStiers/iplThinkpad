#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"
#include "clientUtils.h"

int main(int argc, char* argv[]){
	
	//int ret;

	structMessage msg;
	
	printf("Bienvenue dans le programme\n");
	
	msg.code = DEMANDE_CONNEXION;
	
	initSocketClient(SERVER_IP, atoi(argv[1]));
	ecrireMessageAuServeur(&msg);
	lireMessageDuServeur(&msg);
	
	if (msg.code == CONNEXION_REUSSIE){
		printf("Réponse du serveur : Inscription acceptée\n");		
	} else {
		printf("Réponse du serveur : Inscription refusée\n");		
	}
	
	exit(0);
}