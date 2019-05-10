#include "serveurUtils.h"
#include "utils.h"
#include "message.h"



int shm_id;
int sem_id;

int initServeur(int port){
	struct sockaddr_in addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
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
  for(int i=0;i<memoirePartagee->tailleLogique; i++){
    if(memoirePartagee->listeProgramme[i].id == id){
      indice = i;
    }
  }
  return indice;
}

void arret_programme(int sig){ // REGARDER LA SOLUTION INTERNET. PLUS CHIANT MAIS PAS MAL
	//int status; unused
	printf("Arr�t en cours, il faut attendre de finir l'execution des programmes fils.\n");
	//waitpid(fils, &status, WNOHANG);    Comment car fils undeclared
	exit(0);
}

//******************************************************************************
//MEMOIRE PARTAGEE
//******************************************************************************

void init_shm(size_t size) {
  shm_id = shmget(KEY_SHM, size, IPC_CREAT | PERM);
  checkNeg(shm_id, "Error shmget");
  memoirePartagee = shmat(shm_id, NULL, 0);
  checkCond(memoirePartagee == (void*) -1, "Error shmat");
}

void shmdtCheck() {
  int r = shmdt(memoirePartagee);
  checkNeg(r, "Error shmdt");
}

void detruire_shm() {
  int r = shmctl(shm_id, IPC_RMID, NULL);
  checkNeg(r, "Error shmctl");
}

//******************************************************************************
//SEMAPHORES
//******************************************************************************

void get_sem(){
  sem_id = semget(KEY_SEM, 0, 0);
  checkNeg(sem_id, "Error semget");
}

void init_sem(int val) {

  sem_id = semget(KEY_SEM, 1, IPC_CREAT | 0666);
  checkNeg(sem_id, "Error semget");
  union semun arg; 
  arg.val = val;
  int rv = semctl(sem_id, 0, SETVAL, arg);
  checkNeg(rv, "Error semctl");
}

void add_sem(int val) {
  struct sembuf sem; 
  sem.sem_num = 0; 
  sem.sem_op = val; 
  sem.sem_flg = 0;
  int rc = semop(sem_id, &sem, 1);
  checkNeg(rc, "Error semop");
}

void down() {
  add_sem(-1);
}

void up() {
  add_sem(1);
}

void del_sem() {
  int rv = semctl(sem_id, 0, IPC_RMID);
  checkNeg(rv, "Error semctl");
}

