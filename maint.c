#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	if(argc > 2){
		//3 delay
	}else{
		switch(atoi(argv[1])){
			case 1:
				init_shm();
				printf("L'initialisation de la mémoire partagée a été effectuée.\n");
				break;
			case 2:
				init_shm();
				printf("yahouu : %d\n", listeProgramme[266]->id);
				shmdtCheck();
				detruire_shm();
				printf("La destruction de la mémoire partagée a été effectuée.\n");
				break;
		}
	}
	exit(0);
}