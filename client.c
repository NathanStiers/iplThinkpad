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
	int pipefdMinuterie[2];
	int pipefdExec[2];
	int ret;
	ret = pipe(pipefdMinuterie);
	checkNeg(ret, "Problème lors du pipe de la minuterie.");
	int fdMinuterie = fork_and_run_arg_arg(&filsMinuterie, &delay, &pipefdMinuterie);

	ret = pipe(pipefdExec);
	checkNeg(ret, "Problème lors du pipe du fils d'éxecution.");
	int fdExec = fork_and_run_arg(&filsExecution, &pipefdExec);
	close(pipefdExec[0]);
	close(pipefdMinuterie[1]);

	printf("Bienvenue dans le programme\n");
	terminal(pipefdMinuterie, pipefdExec, fdMinuterie, fdExec);
	exit(0);
}

/**
 * permet à l'utilisateur de rentrer des commandes.
 * */
void terminal(int pipefdMinuterie[], int pipefdExec[], int fdMinuterie, int fdExec)
{
	int fdFichier;
	int tabEcoute[TABECOUTE];
	int idMinuterie[MAXPROGS];
	int tailleLogiqueMinuterie = 0;
	tabEcoute[0] = 0;
	tabEcoute[1] = pipefdMinuterie[0];
	int ret;
	char buffer[MAX_LONGUEUR];
	char *bufferTemp;
	int nbChar;
	//char nbchar;
	afficherMessageCmd();
	fd_set monSet;
	FD_ZERO(&monSet);
	for (int i = 0; i < TABECOUTE; i++)
	{
		FD_SET(tabEcoute[i], &monSet);
	}
	while (1)
	{
		ret = select(FD_SETSIZE, &monSet, NULL, NULL, NULL);
		checkNeg(ret, "Erreur select");
		for (int i = 0; i < TABECOUTE; i++)
		{
			if (FD_ISSET(tabEcoute[i], &monSet))
			{
				switch (i)
				{
				case 0: // Cas user terminal
					ret = read(tabEcoute[i], &buffer, MAX_LONGUEUR);
					checkNeg(ret, "Erreur de read dans le cas terminal (PERE)");
					bufferTemp = strtok(buffer, " ");
					switch (*bufferTemp)
					{
					case '+': // Ajoute un fichier C sur le serveur.
						connexionServeur();
						msg.code = AJOUT;
						bufferTemp = strtok(NULL, ".");
						strcat(bufferTemp, ".c");
						strcpy(msg.nomFichier, bufferTemp);
						fdFichier = open(bufferTemp, O_RDONLY, 0444);
						checkNeg(fdFichier, "Erreur lors de l'ouverture du fichier.");
						while ((nbChar = (read(fdFichier, &buffer, MAX_LONGUEUR)) != 0))
						{
							msg.nbChar = nbChar;
							strcpy(msg.MessageText, buffer);
							printf("%d \n", nbChar);
							ecrireMessageAuServeur(&msg);
						}
						shutdown(sockfd, SHUT_WR);
						closeCheck(fdFichier);
						lireMessageDuServeur(&msg); // doit lire plusieurs fois ... Aussi shutdown depuis le serveur ?
						printf("\n\n**************************************************\n\n%s\n\n**************************************************\n\n", msg.MessageText);
						closeCheck(sockfd);
						break;
					case '*': // Transmet le programme à exec par la minuterie.
						bufferTemp = strtok(NULL, " ");
						idMinuterie[tailleLogiqueMinuterie] = strtol(bufferTemp, NULL, 0);
						tailleLogiqueMinuterie++;
						break;
					case '@': // Demande d'exec un programme au serveur.
						bufferTemp = strtok(NULL, " ");
						msg.idProgramme[0] = strtol(bufferTemp, NULL, 0);
						msg.nbProgrammes = 1;
						msg.code = EXEC;
						write(pipefdExec[1], &msg, sizeof(msg));
						break;
					case 'q': // Déconnecte le client et libère les ressources.
						kill(fdMinuterie, SIGKILL);
						kill(fdExec, SIGKILL);
						exit(0);
						break;
					}
					afficherMessageCmd();
					break;
				case 1: // Cas mintuerie
					ret = read(tabEcoute[i], &msg, sizeof(msg));
					checkNeg(ret, "Erreur read dans la minuterie");
					if (msg.code == MINUTERIE)
					{
						msg.nbProgrammes = tailleLogiqueMinuterie;
						for (int i = 0; i < tailleLogiqueMinuterie; i++)
						{
							msg.idProgramme[i] = idMinuterie[i];
						}
						write(pipefdExec[1], &msg, sizeof(msg));
					}
					break;
				}
			}
		}
		// Reset du select (A demander si possible de faire mieux)
		FD_ZERO(&monSet);
		for (int i = 0; i < TABECOUTE; i++)
		{
			FD_SET(tabEcoute[i], &monSet);
		}
	}
	
}

/**
 * Ecris toutes les x temps sur le pipefd vers le père.
 * */
void filsMinuterie(int *delay, int pipefdMinuterie[])
{
	structMessage msg;
	msg.code = 10;
	close(pipefdMinuterie[0]); // Faudrait pas le mettre en pause tant que le client n'a pas envoyé la commande * ?
	while (1)
	{
		sleep(*delay);
		write(pipefdMinuterie[1], &msg, sizeof(msg));
	}
}

/**
 * Demande au serveur d'executer un programme.
 * */
void filsExecution(int pipefdExec[])
{
	close(pipefdExec[1]);
	int ret;
	structMessage msg;
	while (1)
	{
		ret = read(pipefdExec[0], &msg, sizeof(msg));
		checkNeg(ret, "Erreur lors du read sur le pipe de l'éxecution.");
		printf("Nb de programmes? %d\n", msg.nbProgrammes);
		for (int i = 0; i < msg.nbProgrammes; i++)
		{
			printf("Id à exec : %d\n", msg.idProgramme[i]);
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

void connexionServeur()
{
	structMessage msg;
	initSocketClient(SERVER_IP, PORT_IP);
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
