#ifndef _MESSAGE_H
#define _MESSAGE_H

#define MAX_LONGUEUR 256
#define MAXPROGS 50
//MESSAGE CODE
#define MINUTERIE 10
#define DEMANDE_CONNEXION 1
#define CONNEXION_REUSSIE 2
#define CONNEXION_ECHOUEE 3
#define AJOUT -1
#define EXEC -2

typedef struct {
	char MessageText[MAX_LONGUEUR];
	int nbChar;
	int code;
	int idProgramme;
	int nbProgrammes;
	char nomFichier[MAX_LONGUEUR];
	int erreurCompil;
	int nbrExec;
	long dureeExecTotal;
} structMessage;

#endif