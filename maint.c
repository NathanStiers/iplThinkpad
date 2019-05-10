#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	if(argc > 2){
		get_sem();
		down();
		sleep(atoi(argv[2]));
		up();
	}else{
		switch(atoi(argv[1])){
			case 1:
				init_sem(1);
				init_shm(sizeof(MemoirePartagee));
				memoirePartagee->tailleLogique = 0;
				printf("L'initialisation de la mémoire partagée a été effectuée.\n");
				break;
			case 2:  // BUG
				get_sem();
				del_sem();
				init_shm(sizeof(MemoirePartagee));
				shmdtCheck();
				detruire_shm();
				
				printf("La destruction de la mémoire partagée a été effectuée.\n");
				break;
		}
	}
	exit(0);
}