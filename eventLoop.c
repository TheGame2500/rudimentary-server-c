#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "eventLoop.h"

#define MAX_QUEUE 200
//prototypes
char* getPath(int socketDescriptor); 
//Things to note :
//1)Need to translate ints to network/host byte-order
//2)Important structs:
//	a) sockaddr_in {
//			short sin_family; //which protocol to use (e.g IPv4 = AF_INET)
//			u_short sin_port; //which port
//			struct in_addr sin_addr;
//			char sin_zero[8];
//		}
//	b) in_addr {
//			u_long s_addr;
//		}

void sig_handler(int signo) {
	printf("Killing server...\n");
}
int listenToPort(int port) {
	int socketDescriptor;
	struct sockaddr_in destAddr;
	int errorCode;

	//server settings
	memset(&destAddr,0, sizeof destAddr);
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(port);
	destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

//1. Create TCP Socket and sockaddr_in struct
	socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socketDescriptor == -1){
		printf("ERROR: cannot create socket\n");
		return -1;
	}
//2. Bind port received as argument to said socket
	errorCode = bind(socketDescriptor, (struct sockaddr *)&destAddr, sizeof destAddr );
	if(errorCode >= 0) {
		errorCode = listen(socketDescriptor, MAX_QUEUE);
		if(errorCode >= 0) { 
			printf("Socket opened on port %i\n",port);
		} else {
			printf("Listen failed\n");
			goto exitGracefully;
		}
	} else {
		printf("Bind failed\n");
		goto exitGracefully;
	}

//3. Handle responses by sending back HTML;
	while(1) {
		printf("Accepting..\n");
		int clientDescriptor = accept(socketDescriptor, NULL, NULL);
		if(clientDescriptor >= 0){
			//simple lazy parsing of the request header, just get the first line and extract file path :)
			char* filePath = getPath(clientDescriptor);
			printf("filePath : %s \n", filePath);
			close(clientDescriptor);
		}
		else {
			printf("Client socket failed : %i\n", errno);
		}

		if(signal(SIGKILL, sig_handler) != SIG_ERR){
			//exit gracefully
			goto exitGracefully;
		}
		
	}
	
exitGracefully:
	if(errorCode != 0)
		printf("ERROR : %i\n", errorCode);
	//close socket if it exists
	if(socketDescriptor != -1)
		close(socketDescriptor);
	return errorCode;
}

char* getPath(int socketDescriptor) {
	//get the whole request header into a buffer so we can have fun with it later
	char* request = (char *)malloc(1);
	char buffer[1024];
	int start = read(socketDescriptor, &buffer, sizeof(buffer)); 
	while( start != 0 && start != -1 ) {
		request = (char *) realloc(request, sizeof(request)+sizeof(buffer));
		request = strcat(request, buffer);
		if(strstr(buffer, "\r\n\r\n") != NULL) break;
		start = read(socketDescriptor, &buffer, sizeof(buffer)); 
	}

	//fun time
	//skip first 4 chars "GET " and then get everything until " "
	int counter = 4;
	static char path[50] = ""; 

	while(request[counter] != ' ') {
		path[ strlen(path) ] = request[counter];
		counter++;
	}

	return path;
}
