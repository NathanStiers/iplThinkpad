#include "serveurUtils.h"
#include "utils.h"

int shm_id;

int initServeur(int port){
	struct sockaddr_in addr;
    // socket creation
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // prepare sockaddr to bind
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // listen on all server interfaces
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    // simultaneous client max -> 5
    listen(sockfd, MAX_UTILISATEURS);
    return sockfd;
}

void lireMessageClient(structMessage *msg, int sockfd){
	int ret = read(sockfd, msg, sizeof(*msg));
	checkNeg(ret, "Erreur de lecture serveur");
}

void ecrireMessageClient(structMessage *msg, int sockfd){
	int ret = write(sockfd, msg, sizeof(*msg));
	checkNeg(ret, "Erreur d'écriture serveur");
}

int contains(int id){
  int indice = -1;
  for(int i=0;i<tailleLogique; i++){
    if(listeProgramme[i]->id == id){
      indice = i;
    }
  }
  return indice;
}

//******************************************************************************
//MEMOIRE PARTAGEE
//******************************************************************************

void init_shm() {
  shm_id = shmget(KEY, sizeof(int), IPC_CREAT | 0666);
  checkNeg(shm_id, "Error shmget");
  
  for(int i=0;i<TAILLEPHYSIQUE;i++){
    listeProgramme[i] = shmat(shm_id, NULL, 0);
    checkCond(listeProgramme == (void*) -1, "Error shmat");
  }
}

void shmdtCheck() {
  int r = shmdt(listeProgramme);
  checkNeg(r, "Error shmdt");
}

void detruire_shm() {
  int r = shmctl(shm_id, IPC_RMID, NULL);
  checkNeg(r, "Error shmctl");
}
