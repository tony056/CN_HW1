#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

/* define area */ 
#define PORT_NUM (5000)


/* global variable */
int listenfd, connectfd;
socklen_t length;
struct sockaddr_in serverAddress, clientAddress;
char* prevPath, currentPaht, initPath;

/* function */
void setServer();
void buildSocketConnect();
void setEnv();
void exec_shell();
void exec_cmd();
void parse_cmd(char *str);

int main(int argc, char *argv[]){

	int pid;
	buildSocketConnect();

	printf("%s\n","Server running...waiting for connections.");



	while(1){
		
		length = sizeof(clientAddress);

		connectfd = accept (listenfd, (struct sockaddr *) &clientAddress, &length);
        printf("%s\n","Received request...");
 		
        if((pid = fork()) < 0 ){
        	//error
        }else if(pid == 0){
        	//child
        	exec_shell();
        }

		//multi children

		
	}

	return 0;
}

void setServer(){

	bzero( &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT_NUM);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
}

void buildSocketConnect(){
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	setServer();
	
	bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(listenfd, 1);
}

void setEnv(){

}

void exec_shell(){

	while((n = recv(connectfd, buffer, MAXLINE, 0)) > 0){
		exec_cmd(parse(cmd));
	}
			//-> execute(parse(cmd))
				// -> fork, pipe 
}