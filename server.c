#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>

int PORT_NUM = 5000;

int main(int argc, char *argv[]){
    char buffer[1024];
    int listenfd, connectfd;
    socklen_t length;
    struct sockaddr_in serverAddress, clientAddress;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero( &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT_NUM);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(listenfd, 1);
    
    length = sizeof(clientAddress);
    connectfd = accept(listenfd, (struct sockaddr*)&clientAddress, &length);
    if(connectfd < 0) {
	printf("accept error\n");
	return 0;
    }
    write(connectfd, "Hello Client\n", 13);
    int read_size = 0;
    while((read_size = recv(connectfd, buffer, 1024, 0))> 0){
	if(strcmp("ls", buffer) == 0){
	    char getString[100] = "we get your ls";
	    write(connectfd, getString , strlen(getString));
	}
	else if(strcmp("cat", buffer) == 0){
	    char getString[100] = "we get your cat";
	    write(connectfd, getString, strlen(getString));
	}
	else if(strcmp("grep", buffer) == 0){	
	    char getString[100] = "we get your grep";
	    write(connectfd, getString, strlen(getString));
	}else{
	    write(connectfd, "Not a command", 13);
	}
	bzero(&buffer, sizeof(buffer));
    }
    
    if(read_size <= 0){
	printf("disconnected\n");
	close(connectfd);
    }
    return 0;
}
