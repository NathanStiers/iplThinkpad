#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	init_shm();
	detruire_shm();
	exit(0);
}