#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"
#include "clientUtils.h"

int pipefdMinuterie[2];
int pipefdExec[2];

int main(int argc, char* argv[]){
	
	int ret, pidMinuterie, pidExec;	
	ret = pipe(pipefdMinuterie);
	checkNeg(ret, "Problème lors du pipe de la minuterie.");
	pidMinuterie = fork_and_run(filsMinuterie);
	close(pipefdMinuterie[1]);
	
	ret = pipe(pipefdExec);
	checkNeg(ret, "Problème lors du pipe du fils d'éxecution.");
	pidExec = fork_and_run(filsExecution);
	close(pipefdExec[0]);
	printf("1 : %d, 2 : %d \n", pidMinuterie, pidExec);

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
	while(1){
		sleep(20);
		printf("ici exec du daron.");
	}
	exit(0);
}

/**
 * Ecris toutes les x temps sur le pipefd vers le père.
 * */
void filsMinuterie(){
	close(pipefdMinuterie[0]);
	while(1){
		sleep(10);
		printf("Fils minuterie\n");
	}
}

/**
 * Demande au serveur d'executer un programme.
 * */
void filsExecution(){
	close(pipefdExec[1]);	
	while(1){
		sleep(25);
		printf("Fils d'éxecution.\n");
	}
}