#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string.h>

#include "utils.h"
#include "clientUtils.h"

#define TABECOUTE 2

int main(int argc, char* argv[]){
	if(argc > 4){
		perror("Nombres de paramètre incorrect : ./client adr port delay");
	}
	int delay = atoi(argv[3]);
	//*****************************************************************************
	// Initialisation des pipes
	//*****************************************************************************
	int pipefdMinuterie[2];
	int pipefdExec[2];
	int ret;	
	ret = pipe(pipefdMinuterie);
	checkNeg(ret, "Problème lors du pipe de la minuterie.");
	fork_and_run_arg_arg(&filsMinuterie, &delay, &pipefdMinuterie);
	
	ret = pipe(pipefdExec);
	checkNeg(ret, "Problème lors du pipe du fils d'éxecution.");
	fork_and_run_arg(&filsExecution, &pipefdExec);
	close(pipefdExec[0]);
	close(pipefdMinuterie[1]);

	structMessage msg;
	
	printf("Bienvenue dans le programme\n");
	
	initSocketClient(SERVER_IP, atoi(argv[2]));
	msg.code = DEMANDE_CONNEXION;
	ecrireMessageAuServeur(&msg);
	lireMessageDuServeur(&msg);
	
	if (msg.code == CONNEXION_REUSSIE){
		printf("Réponse du serveur : Inscription acceptée\n");	
		terminal(pipefdMinuterie);	
	} else {
		printf("Réponse du serveur : Inscription refusée\n");		
	}
	exit(0);
}

/**
 * permet à l'utilisateur de rentrer des commandes.
 * */
void terminal(int pipefdMinuterie[]){
	int fdFichier;
	structMessage msg;
	int idMintuerie = -1;
	int tabEcoute[TABECOUTE];
	tabEcoute[0] = 0;
	tabEcoute[1] = pipefdMinuterie[0];
	int ret;
	char buffer[MAX_LONGUEUR];
	char* bufferTemp;
	//char nbchar;
	afficherMessageCmd();
	fd_set monSet;
	FD_ZERO(&monSet);
	for(int i=0;i<TABECOUTE;i++){
		FD_SET(tabEcoute[i], &monSet);
	}
	while(1){
		ret = select(FD_SETSIZE, &monSet, NULL, NULL, NULL);
		checkNeg(ret, "Erreur select");
		for(int i=0;i<TABECOUTE;i++){
			if(FD_ISSET(tabEcoute[i], &monSet)){
				switch(i){ 
					case 0: // Cas user terminal
						ret = read(tabEcoute[i], &buffer, MAX_LONGUEUR);
						checkNeg(ret, "Erreur de read dans le cas terminal (PERE)");
						bufferTemp = strtok(buffer, " ");
						switch(*bufferTemp){
							case '+': // Ajoute un fichier C sur le serveur.
								bufferTemp = strtok(NULL,".");
								fdFichier = open(strcat(bufferTemp,".c"), O_RDONLY, 0444);
								checkNeg(fdFichier, "Erreur lors de l'ouverture du fichier.");
								ret = read(fdFichier, &buffer, MAX_LONGUEUR);
								checkNeg(ret, "Erreur lors de la lecture du fichier.");
								printf("Contenu du fichier : %s\n",buffer);

								msg.code = AJOUT;
								close(fdFichier);
								ecrireMessageAuServeur(&msg);
								break;
							case '*': // Transmet le programme à exec par la minuterie.
								bufferTemp = strtok(NULL, " ");
								idMintuerie = strtol(bufferTemp, NULL, 0);
								printf("test le stockage de l'id : %d \n",idMintuerie);
								break;
							case '@': // Demande d'exec un programme au serveur.
								bufferTemp = strtok(NULL, " "); 
								break;
							case 'q': // Déconnecte le client et libère les ressources.
								break;
						}
						afficherMessageCmd();
					break;
					case 1: // Cas mintuerie
						ret = read(tabEcoute[i], &msg, sizeof(msg));
						if(msg.code == MINUTERIE){
							printf("Réception du message de la minuterie.\n");
						}
					break;
				}
			}
			// Reset du select (A demander si possible de faire mieux)
			FD_ZERO(&monSet);
			for(int i=0;i<TABECOUTE;i++){
				FD_SET(tabEcoute[i], &monSet);
			}
		}
	}
}

/**
 * Ecris toutes les x temps sur le pipefd vers le père.
 * */
void filsMinuterie(int* delay, int pipefdMinuterie[]){
	structMessage msg;
	msg.code = 10;
	close(pipefdMinuterie[0]);
	while(1){
		sleep(*delay);
		write(pipefdMinuterie[1], &msg, sizeof(msg));
	}
}

/**
 * Demande au serveur d'executer un programme.
 * */
void filsExecution(int pipefdExec[]){
	close(pipefdExec[1]);	
	while(1){
	}
}

/**
 * Affiche sur le terminal les commandes disponnibles
 * */
void afficherMessageCmd(){
	printf("**************************************************\n");
	printf("* Liste des commandes utilisables :\n");
	printf("* Ajout d'un fichier : + <chemin d’un fichier C>\n");
	printf("* Programme pour la minuterie : * num\n");
	printf("* Demande l'éxecution d'un programme : @ num\n");
	printf("* Déconnection : q\n");
	printf("***************************************************\n");
}
