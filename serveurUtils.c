#include "serveurUtils.h"

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

void lireMessageClient(structMessage * msg, int sockfd){
	int ret = read(sockfd, msg, sizeof(*msg));
	checkNeg(ret, "Erreur de lecture serveur");
}

void ecrireMessageClient(structMessage * msg, int sockfd){
	int ret = write(sockfd, msg, sizeof(*msg));
	checkNeg(ret, "Erreur d'écriture serveur");
}