#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

/* define area */ 
#define PORT_NUM (5000)
#define MAX_CMD_COUNT (15)
#define MAX_CMD_LENGTH (100)

/* global variable */
int listenfd, connectfd;
socklen_t length;
struct sockaddr_in serverAddress, clientAddress;
char* prevPath, currentPaht, initPath;
char* buffer;
int pipe_length = 0;
int arg_range = 0;
int advance = 0;
int index =0 ;
int parse = 0;

/* function */
void setServer();
void buildSocketConnect();
void setEnv();
void exec_shell();
void exec_cmd();
void parse_cmd(char *str);

int main(int argc, char *argv[]){

	pid_t pid;
	buildSocketConnect();

	printf("%s\n","Server running...waiting for connections.");


	memset(buffer, '\0', sizeof(buffer));
	while(1){
		
		length = sizeof(clientAddress);

		connectfd = accept (listenfd, (struct sockaddr *) &clientAddress, &length);
        printf("%s\n","Received request...");
 		
        if((pid = fork()) < 0 ){
        	printf("server fork error\n");
        	return 0;
        	//error
        }else if(pid == 0){
        	//child
        	exec_shell();
        }


		
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
		exec_cmd(parse(buffer));
	}
			//-> execute(parse(cmd))
				// -> fork, pipe 
}
void exec_cmd(char** arg, int in, int out){
	parse++;
	int pipes_fd[2], v = 0, m = 0;
	char exe_file[50];
	int fd_in;
	int fd_out = (parse == ) ? ()
	//dup2(fd_in, STDIN_FILENO);
	//dup2(fd_out, STDOUT_FILENO);
	if(pipe(pipes_fd) == -1){
		
		fprintf(stderr, "Error: Unable to create pipe. \n");
        exit(EXIT_FAILURE);
	}

	pid_t pid = fork();

	if(pid < 0){

		fprintf(stderr, "Error: Unable to fork.\n");
        exit(EXIT_FAILURE);
	}else if(pid == 0){
		close(pipes_fd[1]);
		dup2(pipes_fd[0])
		//child
		parse_cmd();
		if(execvp(arg[0], arg) == -1){
			
			fprintf(stderr,
                    "Error: Unable to load the executable %s.\n",
                    argv[0]);
            exit(EXIT_FAILURE);
		}
	}else{
		close(pipes_fd[0]);
		//wait(pid, NULL, 0);
	}
}
void parse_cmd(){
	
	char* arg[MAX_CMD_COUNT];
	int i = 0, q_flag = 0;
	
	int cmd_index = 0, cmd_length = 0;
	memset(arg, '\0', sizeof(arg));
	for(i = index; i < strlen(cmd); i++){
		
		if(str[i] == '>'){
			//parse file name, create, and break

		}
		
		if(str[i] == '|'){

			if( (q_flag % 2) == 0){

				index = i+1;
				exec_cmd(arg);
				continue;
			
			}
		}

		if(str[i]!= ' '){
			arg[cmd_index][cmd_length] = str[i];
			cmd_length++;
		}else{

			if(str[i-1] != ' '){
				if(str[i+1] != '|'){
					
					cmd_index++;
				}
				
			}
		
		}

		if(str[i] == '\''){
			q_flag++;
		}

	}


}