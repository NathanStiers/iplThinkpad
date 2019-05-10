#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	init_shm(sizeof(MemoirePartagee));
	int indice = contains(atoi(argv[1]));
	checkNeg(indice, "Aucun programme contenant cet indice");
	Programme p = memoirePartagee->listeProgramme[indice];
	printf("[Id : %d]\n", p.id);
	printf("[Nom du fichier : %s]\n", p.nomFichier);
	printf("[Erreur de compilation : %d]\n", p.erreurCompil);
	printf("[Nombre d'éxecution : %d]\n", p.nbrExec);
	printf("[Durée d'éxecution totale : %ld]\n", p.dureeExecTotal);
	exit(0);
}