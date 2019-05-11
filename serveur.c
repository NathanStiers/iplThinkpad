#include <errno.h>
#include "utils.h"
#include "serveurUtils.h"
#include "message.h"

int nbrFils = 0;
bool isInterupt = true;
int tempsExec;
int status;
void execute(char nomFichier[MAX_LONGUEUR]);

void handler_empty(int sig);

void handler_sigaction(int sig);

void handler_fork(int *sockfd);

void handler2(char nomFichier[MAX_LONGUEUR]);

void handler1(char nomFichier[MAX_LONGUEUR]);

int compile(char nomFichier[MAX_LONGUEUR]);

int main(int argc, char *argv[])
{
	get_sem();
	init_shm(sizeof(MemoirePartagee));

	int sockfd, newsockfd, ret;
	structMessage message;
	//Sigaction
	struct sigaction newact = {{0}};
	newact.sa_handler = handler_sigaction;
	ret = sigaction(SIGCHLD, &newact, NULL);
	checkNeg(ret, "Erreur de sigaction");
	ret = sigaction(SIGINT, &newact, NULL);
	checkNeg(ret, "Erreur de sigaction");
	//Preparation bloquage SIGINT dans les fils
	sigset_t newmask, oldmask;
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);
	//Init du serv
	sockfd = initServeur(atoi(argv[1]));
	printf("Serveur lancé sur le port : %d\n", atoi(argv[1]));

	while (isInterupt)
	{
		printf("Actuellement : %d connexions\n", nbrFils);
		if (nbrFils < 50)
		{
			newsockfd = accept(sockfd, NULL, NULL);
			if (newsockfd > 0)
			{
				lireMessageClient(&message, newsockfd);
				if (message.code == DEMANDE_CONNEXION)
				{
					message.code = CONNEXION_REUSSIE;
					ecrireMessageClient(&message, newsockfd);
				}
				down();
				nbrFils++;
				up();
				sigprocmask(SIG_BLOCK, &newmask, &oldmask);
				fork_and_run_arg(handler_fork, &newsockfd);
				sigprocmask(SIG_SETMASK, &oldmask, NULL);
			}
		}
	}
	printf("%d fils à attendre\n", nbrFils);
	while (nbrFils != 0)
	{
		wait(&status);
	}
	exit(1);
}

void handler_fork(int *newsockfd)
{
	structMessage msg;
	lireMessageClient(&msg, *newsockfd);
	char nomNouveauFichier[MAX_LONGUEUR];
	int titreConnu = 0;
	int fdFichierNouveau = -1;
	char concatName[MAX_LONGUEUR] = "programmes/";
	//int numProg = -1;
	int tailleLogiqueTemp = 0;
	int fdopen;
	int nbLut = 0;
	int contient = 0;
	int ret;
	switch (msg.code)
	{
	case AJOUT:
		// écrit le fichier dans un folder
		if (!titreConnu)
		{
			titreConnu = 1;
			down();
			memoirePartagee->tailleLogique++;
			tailleLogiqueTemp = memoirePartagee->tailleLogique;
			up();
			strcpy(nomNouveauFichier, itoa(tailleLogiqueTemp, nomNouveauFichier));
			strcat(nomNouveauFichier, ".c");
			strcat(concatName, nomNouveauFichier);
			fdFichierNouveau = openConfig(concatName);
		}
		while ((ret = read(*newsockfd, &msg, sizeof(msg))) != 0)
		{
			read(*newsockfd, &nbLut, sizeof(int));
			write(fdFichierNouveau, msg.MessageText, nbLut);
		}
		status = compile(concatName);
		Programme p;
		p.id = tailleLogiqueTemp;
		strcpy(p.nomFichier, msg.nomFichier);
		if (status == 0)
		{
			p.erreurCompil = 0;
		}
		else
		{
			p.erreurCompil = 1;
		}
		p.nbrExec = 0;
		p.dureeExecTotal = 0;
		memoirePartagee->listeProgramme[tailleLogiqueTemp-1] = p;
		
		down();
		fdopen = open("res_compile.txt", 0444);
		checkNeg(fdopen, "Impossible de lire les erreurs\n");
		msg.idProgramme = tailleLogiqueTemp;
		while ((nbLut = read(fdopen, &msg.MessageText, MAX_LONGUEUR)) != 0)
		{	
			msg.nbChar = nbLut;
			write(*newsockfd, &msg, sizeof(msg));
		}
		up();
		shutdown(*newsockfd, SHUT_WR);
		printf("Ajout terminé !\n");
		break;
	case EXEC:
		msg.dureeExecTotal = -1;
		msg.codeRetourProgramme = -1;
		contient = contains(msg.idProgramme);
		if (contient == -1)
		{
			msg.code = -2;
			printf("Le programme n'a pas été ajouté avant d'être exécuté\n");
			ecrireMessageClient(&msg, *newsockfd);
			printf("Execution terminée !\n");
			break;
		}
		strcat(concatName, itoa(memoirePartagee->listeProgramme[contient].id, nomNouveauFichier));
		strcat(concatName, ".c");
		fdopen = open(concatName, O_CREAT | O_EXCL, 0666);
		if (memoirePartagee->listeProgramme[contient].erreurCompil == 0)
		{
			printf("%s\n", concatName);
			execute(concatName);
			down();
			fdopen = open("res_exec.txt", 0444);
			checkNeg(fdopen, "Impossible de lire les sorties\n");
			msg.code = 0;
			if (!status)
			{
				msg.code = 1;
			}
			msg.dureeExecTotal = tempsExec;
			memoirePartagee->listeProgramme[contient].dureeExecTotal += tempsExec;
			memoirePartagee->listeProgramme[contient].nbrExec++;
			msg.codeRetourProgramme = status;
			while ((nbLut = read(fdopen, &msg.MessageText, MAX_LONGUEUR)) != 0)
			{
				msg.nbChar = nbLut;
				write(*newsockfd, &msg, sizeof(msg));
			}
			up();
			shutdown(*newsockfd, SHUT_WR);
		}
		else
		{
			msg.code = -1;
			memoirePartagee->listeProgramme[contient].erreurCompil = 1;
			printf("Le programme ne peut pas être compilé\n");
			ecrireMessageClient(&msg, *newsockfd);
		}
		close(fdopen);
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

int compile(char nomFichier[MAX_LONGUEUR])
{
	int fd = open("res_compile.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
	checkNeg(fd, "ERROR open");
	int stderr_copy = dup(2);
	checkNeg(stderr_copy, "ERROR dup");

	int ret = dup2(fd, 2);
	checkNeg(ret, "ERROR dup2");
	fork_and_run_arg(handler1, nomFichier);
	int status;
	wait(&status);
	printf("test du status : %d", status);
	ret = dup2(stderr_copy, 2);
	checkNeg(ret, "ERROR dup");
	close(stderr_copy);
	return status;
}

void handler_sigaction(int sig)
{
	if (sig == SIGCHLD)
	{
		nbrFils--;
	}
	if (sig == SIGINT)
	{
		isInterupt = false;
	}
}

void handler2(char nomFichier[MAX_LONGUEUR])
{
	char execFile[MAX_LONGUEUR];
	strcpy(execFile, nomFichier);
	strtok(execFile, ".c");
	execl(execFile, nomFichier, NULL);
	perror("Error exec 2");
}

void execute(char nomFichier[MAX_LONGUEUR])
{
	int fd = open("res_execute.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
	checkNeg(fd, "ERROR open");
	int stdout_copy = dup(1);
	checkNeg(stdout_copy, "ERROR dup");
	int ret = dup2(fd, 1);
	checkNeg(ret, "ERROR dup2");
	long t1 = now();
	fork_and_run_arg(handler2, nomFichier);
	wait(&status);
	long t2 = now();
	tempsExec = t2 - t1;
	ret = dup2(stdout_copy, 1);
	checkNeg(ret, "ERROR dup");
	close(stdout_copy);
}