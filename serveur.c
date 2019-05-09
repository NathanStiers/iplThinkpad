#include <sys/select.h>
#include "utils.h"
#include "serveurUtils.h"
#include "message.h"

int main(int argc, char *argv[])
{

	int sockfd, newsockfd, ret;
	int nbrConnexion = 0;
	int connexions[MAX_UTILISATEURS];
	structMessage msg;
	
	//Init du serv
	sockfd = initServeur(atoi(argv[1]));
	printf("Serveur lancée sur le port : %d\n", atoi(argv[1]));
	init_shm();

	fd_set rfds;
	FD_ZERO(&rfds);
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	while (1)
	{
		printf("attente...\n");
		newsockfd = accept(sockfd, NULL, NULL);
		if (newsockfd > 0)
		{
			lireMessageClient(&msg, newsockfd);
			if (msg.code == DEMANDE_CONNEXION)
			{
				connexions[nbrConnexion] = newsockfd;
				FD_SET(connexions[nbrConnexion], &rfds);
				nbrConnexion++;

				msg.code = CONNEXION_REUSSIE;
				ecrireMessageClient(&msg, newsockfd);
				printf("Connexion de %d:\n", connexions[nbrConnexion - 1]);
				printf("Il y a : %d connexions.\n", nbrConnexion);
			}
		}
		ret = select(FD_SETSIZE, &rfds, NULL, NULL, &timeout);
		checkNeg(ret, "Erreur select");
		for (int i = 0; i < nbrConnexion; i++)
		{
			if (FD_ISSET(connexions[i], &rfds))
			{	
				lireMessageClient(&msg, connexions[i]);
				char* nomProgramme = "";
				int titreConnu = 0;
				int fdFichierNouveau = -1;
				char concatName[255] = "programmes/";
				int numProg = -1;
				int status;
				//char useless[5];
				int fdopen;
				switch (msg.code)
				{
				case AJOUT:
					// écrit le fichier dans un folder
					if(!titreConnu){
						titreConnu = 1;
						strcat(concatName, msg.nomFichier);
						fdFichierNouveau = openConfig(concatName);
						write(fdFichierNouveau, msg.MessageText, strlen(msg.MessageText));
					}
						
					while((read(connexions[i], &msg, sizeof(msg))) != 0){
						write(fdFichierNouveau, msg.MessageText, strlen(msg.MessageText));
					}
					//lseek(fdFichierNouveau,-2,SEEK_END);
					//dprintf(fdFichierNouveau, '\0');  // SUPPRIMER CE PUTAIN DE H !!
					//lseek(fdFichierNouveau,1,SEEK_END); 
					
			  
					printf("**************************\n");
					printf("REDIRECTION DE STDERR\n");
					printf("**************************\n");
					int stderr_copy = dup(2);
					checkNeg(stderr_copy, "ERROR dup");
	
					fork_and_run_arg(ajoutCompile, nomProgramme);
					wait(&status);
					
					//int fdErreur = open();
					//while
					Programme p;
					p.id = tailleLogique;
					strcpy(p.nomFichier, nomProgramme); // Il faudrait p-e ajouter l'id au nom de fichier pour éviter les collisions.
					p.erreurCompil = 0;
					p.nbrExec = 0;
					p.dureeExecTotal = 0;
					down();
					*listeProgramme[tailleLogique] = p;
					up();
					tailleLogique++;
					
					fdopen = open(ERREUR_TO_SEND, 0444);
					checkNeg(fdopen, "Impossible de lire les erreurs\n");
					while (read(fdopen, &msg.MessageText, MAX_LONGUEUR) != 0)
						{
							write(connexions[i], msg.MessageText, strlen(msg.MessageText));
						}
					printf("ajout effectué\n");
					printf("**************************\n");
					printf("RÉTABLISSEMENT DE STDERR\n");
					printf("**************************\n");
					ret = dup2(stderr_copy, 2);
					checkNeg(ret, "ERROR dup");
					close(stderr_copy);
					break;
				case EXEC:
					lireMessageClient(&msg, connexions[i]);
					numProg = msg.idProgramme[0];
					down();
					if(contains(numProg) == -1){
						msg.code = -2;
						msg.idProgramme[0] = numProg;
						ecrireMessageClient(&msg, connexions[i]);
						up();
						break;
					}
					up();
					printf("exec\n");
					break;
				}
			}
			// pareil que dans le client, reset le select
			FD_ZERO(&rfds);
			for (int i = 0; i < nbrConnexion; i++)
			{
				FD_SET(connexions[i], &rfds);
			}
		}
	}
}