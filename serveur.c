#include <sys/select.h>
#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char *argv[])
{

	int sockfd, newsockfd, ret;
	int nbrConnexion = 0;
	int connexions[MAX_UTILISATEURS];
	structMessage msg;

	//Init du serv
	sockfd = initServeur(atoi(argv[1]));
	printf("Serveur lancée sur le port : %d\n", atoi(argv[1]));

	fd_set rfds;
	FD_ZERO(&rfds);
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	while (1)
	{
		printf("attente...\n");
		newsockfd = accept(sockfd, NULL, NULL);
		if (newsockfd > 0)
		{
			lireMessageClient(&msg, newsockfd);
			if (msg.code == DEMANDE_CONNEXION)
			{
				connexions[nbrConnexion] = newsockfd;
				FD_SET(connexions[nbrConnexion], &rfds);
				nbrConnexion++;

				msg.code = CONNEXION_REUSSIE;
				ecrireMessageClient(&msg, newsockfd);
				printf("Connexion de %d:\n", connexions[nbrConnexion - 1]);
				printf("Il y a : %d connexions.\n", nbrConnexion);
			}
		}
		ret = select(FD_SETSIZE, &rfds, NULL, NULL, &timeout);
		checkNeg(ret, "Erreur select");
		for (int i = 0; i < nbrConnexion; i++)
		{
			if (FD_ISSET(connexions[i], &rfds))
			{	
				printf("lourd\n");
				lireMessageClient(&msg, connexions[i]);
				switch (msg.code)
				{
				case AJOUT:
					printf("ajout\n");
					break;
				case EXEC:
					printf("exec\n");
					break;
				}
			}
			// pareil que dans le client, reset le select
			FD_ZERO(&rfds);
			for (int i = 0; i < nbrConnexion; i++)
			{
				FD_SET(connexions[i], &rfds);
			}
		}
	}
}