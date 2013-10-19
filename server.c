#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>


#define MAXLINE 1024
int PORT_NUM = 5000;
int dataLength = 0;
char* path = "PATH";
int commandFlag = 0;
char *reply;
int i;

char save_token[15][100];

void judgeToken(char* command);
void execPrintEnv(char* var);
void execSetEnv(char* var);


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
	
    char* prevPath;
    char* currentPath;
    prevPath = getenv (path);
    // if (prevPath!=NULL)
    //     printf ("The current path is: %s",prevPath);
    
    char* initPath = "bin:.";
    if(setenv(path, initPath, 1) != 0){
        printf("set PATH error\n");
        return 0;
    }
    currentPath = getenv(path);
    if (currentPath!=NULL)
        printf ("The current path is: %s",currentPath);

    

    //return 0;

    bind(listenfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(listenfd, 1);

    

    
    printf("%s\n","Server running...waiting for connections.");

	
    
    //bzero(&buffer, sizeof(buffer));

    int n = 0;
    while(1){

        length = sizeof(clientAddress);
        connectfd = accept (listenfd, (struct sockaddr *) &clientAddress, &length);
        printf("%s\n","Received request...");

        while ( (n = recv(connectfd, buffer, MAXLINE, 0)) > 0)  {
           printf("%s","String received from and resent to the client:");
           puts(buffer);

           char *token;
           int tokenNum = 0;
           reply = malloc(strlen(buffer)+1);
           token = strtok(buffer, " \n");
           while(token != NULL){
                strcpy(save_token[tokenNum++], token);
                //strcat(new_string, token);
                token = strtok(NULL, " \n");
           }
           printf("%d\n", tokenNum);
           
           for(i = 0; i < tokenNum; i++){
                judgeToken(save_token[i]);
           }
           //bzero(&buffer, sizeof(buffer));
           send(connectfd, reply, n+1, 0);
        }

        if (n < 0) {
          perror("Read error");
          exit(1);
        }
        close(connectfd);

    }
    close(listenfd);
    if(strcmp(currentPath, initPath) == 0){
        setenv(path, prevPath, 1);
    }
    return 0;
}


void judgeToken(char* command){
    printf("%s,%s\n","judge.....",command );
    printf("commandFlag: %d\n", commandFlag);
    
        
    if(strcmp(command, "printenv") == 0){
        commandFlag = 1;

    }else if(strcmp(command, "setenv") == 0){
        commandFlag = 2;

    }else if(strcmp(command, "PATH") == 0){
        if(commandFlag == 1){
            execPrintEnv(command);
        }else if (commandFlag == 2)
        {
            execSetEnv(command);
        }
        commandFlag = 0;
    }else if(strcmp(command, "ls") == 0){

    }else if(strcmp(command, "cat") == 0){

    }else if(strcmp(command, "grep") == 0){

    }else if(strcmp(command, "|") == 0){

    }
}

void execPrintEnv(char* var){
    char *envPath;
    envPath = getenv(var);
    printf("%s\n", envPath);
    strcpy(reply, "PATH=");
    strcat(reply, envPath);
}
void execSetEnv(char* var){
    char *envPath;
    setenv(var, save_token[++i], 1);
    envPath = getenv(var);
    strcpy(reply, "PATH=");
    strcat(reply, envPath);
}