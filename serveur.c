#include <sys/select.h>
#include <errno.h>
#include "utils.h"
#include "serveurUtils.h"
#include "message.h"


void handler_fork(int* sockfd);

void handler2();

void handler1();

void compile(char nomFichier[MAX_LONGUEUR]);

void execute(char nomFichier[MAX_LONGUEUR]);

int status;

long tempsExec;

int main(int argc, char *argv[])
{
	get_sem();
	init_shm(sizeof(MemoirePartagee));
	int sockfd, newsockfd;
	structMessage message;
	//int nbrConnexion = 0;

	//Init du serv
	sockfd = initServeur(atoi(argv[1]));
	printf("Serveur lancé sur le port : %d\n", atoi(argv[1]));

	while (1)
	{
		printf("attente...\n");
		newsockfd = accept(sockfd, NULL, NULL);
		if (newsockfd > 0)
		{
			lireMessageClient(&message, newsockfd);
			if (message.code == DEMANDE_CONNEXION)
			{
				message.code = CONNEXION_REUSSIE;
				ecrireMessageClient(&message, newsockfd);
			}
			fork_and_run_arg(handler_fork, &newsockfd);
		}
	}
}

void handler_fork(int* newsockfd)
{
	structMessage msg;
	lireMessageClient(&msg, *newsockfd);
	char nomNouveauFichier[MAX_LONGUEUR];
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
			strcpy(nomNouveauFichier, itoa(memoirePartagee->tailleLogique + 1, nomNouveauFichier));
			strcat(nomNouveauFichier, ".c");
			strcat(concatName, nomNouveauFichier);
			fdFichierNouveau = openConfig(concatName);
			write(fdFichierNouveau, msg.MessageText, msg.nbChar);
		}
		
		while ((read(*newsockfd, &msg, sizeof(msg))) != 0)
		{
			write(fdFichierNouveau, msg.MessageText, msg.nbChar);
		}
		Programme p;
		p.id = memoirePartagee->tailleLogique+1;
		strcpy(p.nomFichier, msg.nomFichier); // Il faudrait p-e ajouter l'id au nom de fichier pour éviter les collisions.
		p.erreurCompil = 0;
		p.nbrExec = 0; // code de retour du pgm
		p.dureeExecTotal = 0;
		down();
		memoirePartagee->listeProgramme[memoirePartagee->tailleLogique] = p;
		memoirePartagee->tailleLogique++;
		up();
		compile(concatName);
		fdopen = open("res_compile.txt", 0444);
		checkNeg(fdopen, "Impossible de lire les erreurs\n");
		msg.idProgramme = memoirePartagee->tailleLogique;
		while ((nbLut = read(fdopen, &msg.MessageText, MAX_LONGUEUR)) != 0)
		{
			msg.nbChar = nbLut;
			write(*newsockfd, &msg, sizeof(msg));
		}
		shutdown(*newsockfd, SHUT_WR);
		printf("Ajout terminé !\n");
		break;
	case EXEC:
		numProg = msg.idProgramme;
		msg.idProgramme = numProg;
		msg.dureeExecTotal = -1;
		down();
		contient = contains(numProg);
		if (contient == -1)
		{
			msg.idProgramme = memoirePartagee->tailleLogique;
			up();
			msg.code = -2;
			printf("Le programme n'a pas été ajouté avant d'être exécuté\n");
			ecrireMessageClient(&msg, *newsockfd);
			printf("Execution terminée !\n");
			break;
		}
		strcat(concatName, itoa(memoirePartagee->listeProgramme[contient].id, nomNouveauFichier));
		strcat(concatName, ".c");
		fdopen = open(concatName, O_CREAT | O_EXCL, 0666);
		errsrv = errno;
		up();
		if (errsrv == EEXIST)
		{
			printf("%s\n", concatName);
			execute(concatName);
			fdopen = open("res_compile.txt", 0444);
			checkNeg(fdopen, "Impossible de lire les sorties\n");
			msg.code = 1; // ou 0 à faire selon statut
			msg.dureeExecTotal = tempsExec;
			msg.nbrExec = status;
			while ((nbLut = read(fdopen, &msg.MessageText, MAX_LONGUEUR)) != 0)
			{
				msg.nbChar = nbLut;
				write(*newsockfd, &msg, sizeof(msg));
			}
			shutdown(*newsockfd, SHUT_WR);
		}
		else
		{
			msg.code = -1;
			printf("Le programme ne peut pas être compilé\n");
			ecrireMessageClient(&msg, *newsockfd);
		}
		printf("Execution terminée !\n");
		break;
	}
}

void handler1(char nomFichier[MAX_LONGUEUR])
{
	char fichierC[MAX_LONGUEUR];
	strcpy(fichierC, nomFichier);
	char *fichier = strtok(nomFichier, ".");
	execl("/usr/bin/gcc", "gcc", "-o", fichier, fichierC, NULL);
	perror("Error execl 1");
}

void handler2(char nomFichier[MAX_LONGUEUR])
{
	char execFile[MAX_LONGUEUR];
	strcpy(execFile, nomFichier);
	strtok(execFile, ".c");
	execl(execFile, nomFichier, NULL);
	perror("Error exec 2");
}

void execute(char nomFichier[MAX_LONGUEUR]){
	int fd = open("res_compile.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
	checkNeg(fd, "ERROR open");
	printf("**************************\n");
	printf("EXÉCUTION\n");
	printf("**************************\n");
	int stdout_copy = dup(1);
	checkNeg(stdout_copy, "ERROR dup");
	int ret = dup2(fd, 1);
	checkNeg(ret, "ERROR dup2");
	long t1 = now();
	fork_and_run_arg(handler2, nomFichier);
	wait(&status);
	long t2 = now();
	tempsExec = t2 - t1;
	//printf("SI %d != 0 ALORS execution ok\n", WIFEXITED(status)); // PLUS QU'A VERIFIER SI C'EST BON
	//printf("SI exécution ok ALORS les statut de l'exécution = %d\n", WEXITSTATUS(status));
	//printf("Le temps d'exécution = %ld\n", t2 - t1);
	ret = dup2(stdout_copy, 1);
	checkNeg(ret, "ERROR dup");
	close(stdout_copy);
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