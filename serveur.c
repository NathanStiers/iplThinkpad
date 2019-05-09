#include <sys/select.h>
#include <errno.h>  
#include "utils.h"
#include "serveurUtils.h"
#include "message.h"

void handler2();

void handler1();

void compile();

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
				char *nomProgramme = "";
				int titreConnu = 0;
				int fdFichierNouveau = -1;
				char concatName[255] = "programmes/";
				int numProg = -1;
				int fdopen;
				int nbLut = 0;
				int contient = 0;
				int errsrv = 0;
				switch (msg.code)
				{
				case AJOUT:
					// écrit le fichier dans un folder
					if (!titreConnu)
					{
						titreConnu = 1;
						strcat(concatName, msg.nomFichier);
						fdFichierNouveau = openConfig(concatName);
						write(fdFichierNouveau, msg.MessageText, msg.nbChar);
					}

					while ((read(connexions[i], &msg, sizeof(msg))) != 0)
					{
						write(fdFichierNouveau, msg.MessageText, msg.nbChar);
					}
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
					compile(concatName);
					fdopen = open("res_compile.txt", 0444);
					checkNeg(fdopen, "Impossible de lire les erreurs\n");
					msg.idProgramme = tailleLogique-1;
					while ((nbLut = read(fdopen, &msg.MessageText, MAX_LONGUEUR)) != 0)
					{
						write(connexions[i], msg.MessageText, nbLut);
					}
					shutdown(connexions[i], SHUT_WR);
					printf("Ajout terminé !\n");
					break;
				case EXEC:
					numProg = msg.idProgramme;
					printf("%d\n", numProg);
					down();
					contient = contains(numProg);
					if (contient == -1)
					{
						msg.code = -2;
						msg.idProgramme = numProg;
						printf("Le programme à pas été ajouté\n");
						ecrireMessageClient(&msg, connexions[i]);
						up();
						break;
					}
					fdopen = open(listeProgramme[contient]->nomFichier, O_CREAT | O_EXCL, 0666);
					errsrv = errno;
					up();
					if(errsrv == EEXIST){
						printf("Existe\n");
					}else{
						printf("Existe pas\n");
					}
					printf("Execution terminée !\n");
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

void handler1(char nomFichier[MAX_LONGUEUR])
{
	char fichierC[MAX_LONGUEUR];
	strcpy(fichierC, nomFichier);
	char * fichier = strtok(nomFichier, ".");
	execl("/usr/bin/gcc", "gcc", "-o", fichier, fichierC, NULL);
	perror("Error execl 1");
}

void handler2() {
  execl("./hello", "hello", NULL);
  perror("Error exec 2");
}

void compile(char nomFichier[MAX_LONGUEUR])
{
	printf("**************************\n");
	printf("CRÉATION DU FICHIER res_compile.txt\n");
	printf("**************************\n");
	int fd = open("res_compile.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
	checkNeg(fd, "ERROR open");

	printf("**************************\n");
	printf("REDIRECTION DE STDERR\n");
	printf("**************************\n");
	int stderr_copy = dup(2);
	checkNeg(stderr_copy, "ERROR dup");

	int ret = dup2(fd, 2);
	checkNeg(ret, "ERROR dup2");

	printf("**************************\n");
	printf("COMPILATION DU FICHIER hello.c\n");
	printf("**************************\n");
	fork_and_run_arg(handler1, nomFichier);
	int status;
	wait(&status);
	printf("SI %d != 0, ALORS regarde dans res_compile.txt\n", WEXITSTATUS(status));

	printf("**************************\n");
	printf("RÉTABLISSEMENT DE STDERR\n");
	printf("**************************\n");
	ret = dup2(stderr_copy, 2);
	checkNeg(ret, "ERROR dup");
	close(stderr_copy);
}