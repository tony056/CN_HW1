#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>

int PORT_NUM = 5000;

int main(int argc, char *argv[] ){
    char buffer[50];
    int sockfd;
    struct sockaddr_in serverAddress;
    char command[1024], reply[1024];
    if(argc < 2 ){
	printf("insert error\n");
	return 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT_NUM);
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    //inet_pton(AF_INET, argv[1], &serverAddress.sin_addr);

    if(connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
	printf("coneection error\n");
	return 0;
    }
    int right = 0
    if(recv(sockfd, reply, 1024, 0) < 0){
	
    }
    while(){
	printf("Enter message: ");
	scanf("%s", command);

	if(send(sockfd, command, strlen(command), 0) < 0){
	    //message send error
	    printf("send error\n");
	    return 0;
	}

	if( recv(sockfd, reply, 1024, 0) < 0){
	    printf("recv error\n");
	    return 0;
	}

	printf("Response: %s\n", reply);
	if(strcmp(reply, "Not a command") == 0)
	    break;
	bzero(&reply, sizeof(reply));
	//close(sockfd);
    }
    
    close(sockfd);

    return 0;
}
