#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	int sockfd, newsockfd;
	int nbrConnexion = 0;
	int connexions[MAX_UTILISATEURS];
	structMessage msg;

	//Init du serv
	sockfd = initServeur(atoi(argv[1]));
	printf("Serveur lancée sur le port : %d\n", atoi(argv[1]));

	while(1){
		printf("attente...\n");
		newsockfd = accept(sockfd, NULL, NULL);
		if(newsockfd>0){
			lireMessageClient(&msg, newsockfd);
			if(msg.code == DEMANDE_CONNEXION){
				connexions[nbrConnexion] = newsockfd;
				nbrConnexion++;

				msg.code = CONNEXION_REUSSIE;
				ecrireMessageClient(&msg,newsockfd);
				printf("Connexion de %d:\n", connexions[nbrConnexion-1]);
				printf("Il y a : %d connexions.\n", nbrConnexion);
			}
		}
	}
	
}