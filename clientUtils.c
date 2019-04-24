#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "clientUtils.h"
#include "utils.h"

int sockfd;

int initSocketClient(char ServerIP[16], int Serverport){
	struct sockaddr_in addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr,0,sizeof(addr));
	addr.sin_family= AF_INET;
	addr.sin_port= htons(Serverport);
	inet_aton(ServerIP,&addr.sin_addr);
	connect(sockfd, (struct sockaddr*) &addr, sizeof(addr));
	return sockfd;
}

void ecrireMessageAuServeur(structMessage* msg){
	int ret = write(sockfd,msg,sizeof(*msg));
	checkNeg(ret,"write client error");
}

void lireMessageDuServeur(structMessage* msg){
	int ret = read(sockfd,msg,sizeof(*msg));
	checkNeg(ret,"read client error");
}