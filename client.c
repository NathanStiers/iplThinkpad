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
#include "message.h"

#define TABECOUTE 2
#define MAXPROGS 50

structMessage msg;

int main(int argc, char *argv[])
{
	if (argc > 4)
	{
		perror("Nombres de paramètre incorrect : ./client adr port delay");
	}
	int delay = atoi(argv[3]);
	//*****************************************************************************
	// Initialisation des pipes
	//*****************************************************************************
	int pipefdExec[2];
	int ret;

	ret = pipe(pipefdExec);
	checkNeg(ret, "Problème lors du pipe du fils d'éxecution.");
	int fdExec = fork_and_run_arg(&filsExecution, &pipefdExec);
	int fdMinuterie = fork_and_run_arg_arg(&filsMinuterie, &delay, &pipefdExec);
	close(pipefdExec[0]);

	printf("Bienvenue dans le programme\n");
	terminal(pipefdExec, fdMinuterie, fdExec);
	exit(0);
}

/**
 * permet à l'utilisateur de rentrer des commandes.
 * */
void terminal(int pipefdExec[], int fdMinuterie, int fdExec)
{
	int fdFichier;
	int ret;
	char buffer[MAX_LONGUEUR];
	char *bufferTemp;
	int nbChar;
	//char nbchar;
	afficherMessageCmd();
	while (1)
	{
		ret = read(0, &buffer, MAX_LONGUEUR);
		checkNeg(ret, "Erreur de read dans le cas terminal (PERE)");
		bufferTemp = strtok(buffer, " ");
		switch (*bufferTemp)
		{
		case '+': // Ajoute un fichier C sur le serveur.
			connexionServeur(sockfd);
			msg.code = AJOUT;
			bufferTemp = strtok(NULL, ".");
			strcat(bufferTemp, ".c");
			strcpy(msg.nomFichier, bufferTemp);
			fdFichier = open(bufferTemp, O_RDONLY, 0444);
			checkNeg(fdFichier, "Erreur lors de l'ouverture du fichier.");
			while ((nbChar = read(fdFichier, &buffer, MAX_LONGUEUR)) != 0)
			{
				msg.nbChar = nbChar;
				strcpy(msg.MessageText, buffer);
				ecrireMessageAuServeur(&msg);
			}
			shutdown(sockfd, SHUT_WR);
			closeCheck(fdFichier);
			while((nbChar = read(sockfd, &msg, sizeof(msg))) != 0){
				ret = write(1, msg.MessageText, msg.nbChar);
			}
			printf("\n Le numéro de votre programme est le : %d\n", msg.idProgramme);
			closeCheck(sockfd);
			break;
		case '*': // Transmet le programme à exec par la minuterie.
			bufferTemp = strtok(NULL, " ");
			msg.idProgramme = strtol(bufferTemp, NULL, 0);
			write(pipefdExec[1], &msg, sizeof(msg));
			break;
		case '@': // Demande d'exec un programme au serveur.
			connexionServeur(sockfd);
			bufferTemp = strtok(NULL, " ");
			msg.idProgramme = strtol(bufferTemp, NULL, 0);
			msg.nbProgrammes = 1;
			msg.code = EXEC;
			ecrireMessageAuServeur(&msg);
			while((nbChar = read(sockfd, &msg, sizeof(msg))) != 0){
				ret = write(1, msg.MessageText, msg.nbChar);
			}
			printf("Execution finie du programme %d\n", msg.idProgramme);
			printf("Avec le code de retour %d\n", msg.nbrExec);
			printf("En seulement %ld ms\n", msg.dureeExecTotal);
			//Reception des msg
			break;
		case 'q': // Déconnecte le client et libère les ressources.
			kill(fdMinuterie, SIGKILL);
			kill(fdExec, SIGKILL);
			exit(0);
			break;
		}
		afficherMessageCmd();
	}
}

/**
 * Ecris toutes les x temps sur le pipefd vers le père.
 * */
void filsMinuterie(int *delay, int pipefdExec[])
{
	structMessage msg;
	int ret;
	msg.code = MINUTERIE;
	close(pipefdExec[0]);
	while (1)
	{
		sleep(*delay);
		ret = write(pipefdExec[1], &msg, sizeof(msg));
		checkNeg(ret, "Erreur lors de l'écriture sur le pipe dans la minuterie.");
	}
}

/**
 * Demande au serveur d'executer un programme.
 * */
void filsExecution(int pipefdExec[])
{
	close(pipefdExec[1]);
	int tabProgrammes[MAXPROGS];
	int tailleLogique = 0;
	int ret;
	structMessage msg;
	while (1)
	{
		ret = read(pipefdExec[0], &msg, sizeof(msg));
		checkNeg(ret, "Erreur lors du read dans le fils d'éxecution.");
		if(msg.code == MINUTERIE){
			connexionServeur(sockfdExec);
			msg.code = EXEC;
			for(int i=0;i<tailleLogique;i++){
				msg.idProgramme = tabProgrammes[i];
				ecrireMessageAuServeur(&msg);
				//Lecutre du résultat.
			}
			closeCheck(sockfdExec);
		}else{
			tabProgrammes[tailleLogique] = msg.idProgramme;
			tailleLogique++;
		}
	}
}

/**
 * Affiche sur le terminal les commandes disponnibles
 * */
void afficherMessageCmd()
{
	printf("**************************************************\n");
	printf("* Liste des commandes utilisables :\n");
	printf("* Ajout d'un fichier : + <chemin d’un fichier C>\n");
	printf("* Programme pour la minuterie : * num\n");
	printf("* Demande l'éxecution d'un programme : @ num\n");
	printf("* Déconnection : q\n");
	printf("***************************************************\n");
}

void connexionServeur(int sockfd)
{
	structMessage msg;
	sockfd = initSocketClient(SERVER_IP, PORT_IP);
	msg.code = DEMANDE_CONNEXION;
	ecrireMessageAuServeur(&msg);
	lireMessageDuServeur(&msg);
	if (msg.code == CONNEXION_REUSSIE)
	{
		printf("Réponse du serveur : Inscription acceptée\n");
	}
	else
	{
		printf("Réponse du serveur : Inscription refusée\n");
	}
}
