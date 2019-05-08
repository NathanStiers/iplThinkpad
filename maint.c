#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	if(argc > 2){
		down();
		sleep(atoi(argv[2]));
		up();
	}else{
		switch(atoi(argv[1])){
			case 1:
				init_sem(1);
				init_shm();
				printf("L'initialisation de la mémoire partagée a été effectuée.\n");
				break;
			case 2:  // BUG
				init_shm();
				shmdtCheck();
				detruire_shm();
				del_sem();
				printf("La destruction de la mémoire partagée a été effectuée.\n");
				break;
		}
	}
	exit(0);
}