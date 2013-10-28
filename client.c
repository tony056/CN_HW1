#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h> 
#include <pthread.h>


#define MAXLINE 1024


int PORT_NUM = 5000;
int sockfd;
/* thread: read from server */
void *readFromServer(){
  char reply[5000];
  while(1){
    memset(reply, '\0', 5000);
    recv(sockfd, reply, sizeof(reply), 0);
    printf("%s", reply);

  }
}

int main(int argc, char *argv[] ){
    char buffer[50];
    pthread_t thread;
    struct sockaddr_in serverAddress;
    char command[1024];
    if(argc < 3 ){
		printf("insert error\n");
		return 0;
    }
    PORT_NUM = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    char *hostname = argv[1];
    char **pptr;
    char ipaddr[32];
    struct hostent *hostptr;
    if((hostptr = gethostbyname(hostname)) == NULL){
    	printf("gethostbyname error for host%s\n", hostname);
    	return 0;
    }
    switch(hostptr->h_addrtype){ 
  		case AF_INET: 
  		case AF_INET6: 
   			pptr=hostptr->h_addr_list; 
   			/*將剛才得到的所有地址都打出來。其中調用了inet_ntop()函數*/ 
   			for(;*pptr!=NULL;pptr++) 
    			printf(" address:%s\n", inet_ntop(hostptr->h_addrtype, *pptr, ipaddr, sizeof(ipaddr))); 
   			break; 
  		default: 
   			printf("unknown address type/n"); 
   			break; 
 	  } 
    //return 0;

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, ipaddr, &serverAddress.sin_addr);
    
    printf("coneection\n");
    
    if(connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
  		printf("coneection error\n");
  		return 0;
    }

  	//bzero(&reply, sizeof(reply));
  	pthread_create(&thread, NULL, readFromServer, NULL);    
  	printf("%% ");
    while (fgets(command, MAXLINE, stdin) != NULL) {
      //printf("%% ");
  		send(sockfd, command, strlen(command), 0);

      bzero(&command, sizeof(command));
      
  	}
    
    close(sockfd);

    return 0;
}
