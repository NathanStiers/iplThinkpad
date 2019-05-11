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

#define MAXPROGS 50

structMessage msg;

int port;

char serveurIp[16];

void filsMinuterie(int *delay, int pipefdMinuterie[]);

void filsExecution(int pipefdExec[]);

void terminal(int pipefdExec[], int fdMinuterie, int fdExec);

void afficherMessageCmd();

int main(int argc, char *argv[])
{
	if (argc > 4)
	{
		perror("Nombres de paramètre incorrect : ./client adr port delay");
	}
	strcpy(serveurIp, argv[1]);
	port = strtol(argv[2], NULL, 0);
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
	closeCheck(pipefdExec[0]);

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
	int nbChar = 0;
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
			connexionServeur(sockfd, serveurIp, port);
			msg.code = AJOUT;
			ecrireMessageAuServeur(&msg);
			bufferTemp = strtok(NULL, ".");
			strcat(bufferTemp, ".c");
			strcpy(msg.nomFichier, bufferTemp);
			fdFichier = open(bufferTemp, O_RDONLY, 0444);
			checkNeg(fdFichier, "Erreur lors de l'ouverture du fichier.");
			while ((nbChar = read(fdFichier, &buffer, MAX_LONGUEUR)) != 0)
			{
				strcpy(msg.MessageText, buffer);
				ecrireMessageAuServeur(&msg);
				write(sockfd, &nbChar, sizeof(int));
			}
			shutdown(sockfd, SHUT_WR);
			closeCheck(fdFichier);
			while ((nbChar = read(sockfd, &msg, sizeof(msg))) != 0)
			{
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
			connexionServeur(sockfd, serveurIp, port);
			bufferTemp = strtok(NULL, " ");
			msg.idProgramme = strtol(bufferTemp, NULL, 0);
			msg.code = EXEC;
			ecrireMessageAuServeur(&msg);
			while ((nbChar = read(sockfd, &msg, sizeof(msg))) != 0)
			{
				if (msg.code != -2 && msg.code != -1)
				{
					ret = write(1, msg.MessageText, msg.nbChar);
					checkNeg(ret, "Erreur write");
				}
				else
				{
					break;
				}
			}
			printf("\n");
			printf("Numéro du programme : %d\n", msg.idProgramme);
			printf("Etat du programme %d\n", msg.code);
			printf("Temps d'éxecution : %ld ms\n", msg.dureeExecTotal);
			printf("Code de retour : %d\n", msg.codeRetourProgramme);
			closeCheck(sockfd);
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
	closeCheck(pipefdExec[0]);
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
	closeCheck(pipefdExec[1]);
	int tabProgrammes[MAXPROGS];
	int tailleLogique = 0;
	int ret;
	int nbChar;
	structMessage msg;
	while (1)
	{
		ret = read(pipefdExec[0], &msg, sizeof(msg));
		checkNeg(ret, "Erreur lors du read dans le fils d'éxecution.");
		if (msg.code == MINUTERIE)
		{
			printf("taille logique =  %d\n",tailleLogique);
			for (int i = 0; i < tailleLogique; i++)
			{
				connexionServeur(sockfdExec, serveurIp, port);
				printf("ça bloque ici\n");
				msg.code = EXEC;
				msg.idProgramme = tabProgrammes[i];
				ecrireMessageAuServeur(&msg);
				while ((nbChar = read(sockfd, &msg, sizeof(msg))) != 0)
				{
					if (msg.code != -2 && msg.code != -1)
					{
						ret = write(1, msg.MessageText, msg.nbChar);
						checkNeg(ret,"Erreur write");
					}
					else
					{
						break;
					}
				}
				printf("\n");
				printf("Numéro du programme : %d\n", msg.idProgramme);
				printf("Etat du programme %d\n", msg.code);
				printf("Temps d'éxecution : %ld ms\n", msg.dureeExecTotal);
				printf("Code de retour : %d\n", msg.codeRetourProgramme);
				closeCheck(sockfdExec);
			}
		}
		else
		{
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
