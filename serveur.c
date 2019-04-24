#include "utils.h"
#include "serveurUtils.h"

int main(int argc, char* argv[]){
	int sockfd, newsockfd, ret;
	//Init du serv
	sockfd = initServeur(argv[1]);

	while(1){
		newsockfd = accept(sockfd, NULL, NULL);
		if(newsockfd>0){
			
		}
	}
	
}