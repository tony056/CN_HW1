#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>

/* define area */ 
#define PORT_NUM (5000)
#define MAX_CMD_COUNT (15)
#define MAX_CMD_LENGTH (100)
#define MAX_LENGTH (1024)

/* global variable */
int listenfd, connectfd;
socklen_t length;
struct sockaddr_in serverAddress, clientAddress;
char* buffer;
int pipe_length = 0;
int arg_range = 0;
int advance = 0;
int parse_index = 0;
int parse = 0;
char* buffer;

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

	buffer = (char *)malloc(sizeof(char) * MAX_LENGTH);
	bzero(&buffer, sizeof(buffer));
	char* initPath = "bin:.";
	setEnv(initPath);
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
        	printf("child\n");
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

void setEnv(char *init){
	char* path = "PATH";
	if(setenv(path, init, 1) != 0){
        printf("set PATH error\n");
        exit(EXIT_FAILURE);
    }
}

void exec_shell(){
	
	buffer = (char *)malloc(sizeof(char)*MAX_LENGTH);
	int n = 0, cmd_pid;
	while((n = recv(connectfd, buffer, MAX_LENGTH, 0)) > 0){
		printf("%s\n", buffer);
		cmd_pid = fork();
		if(cmd_pid < 0){
			printf("server fork error\n");
        	exit(EXIT_FAILURE);
		}else if(cmd_pid == 0){
			parse_cmd(buffer);	
		}
		bzero(&buffer, sizeof(buffer));
	}
			//-> execute(parse(cmd))
				// -> fork, pipe 
}
void exec_cmd(char** arg){
	parse++;
	int pipes_fd[2];

	
	if(arg == NULL){
		return;
	}
	if(pipe(pipes_fd) == -1){
		
		fprintf(stderr, "Error: Unable to create pipe. \n");
        exit(EXIT_FAILURE);
	}

	pid_t pid = fork();

	if(pid < 0){

		fprintf(stderr, "Error: Unable to fork.\n");
        exit(EXIT_FAILURE);
	
	}else if(pid == 0){
		
		
		dup2(pipes_fd[0], STDIN_FILENO);
		dup2(pipes_fd[1], STDOUT_FILENO);
		close(pipes_fd[1]);
		//child
		parse_cmd(buffer);

		
	}else{

		close(pipes_fd[0]);
		dup2(pipes_fd[1], STDOUT_FILENO);

		if(execvp(arg[0], arg) == -1){
			
			fprintf(stderr, "Error: Unable to load the executable \n");
            exit(EXIT_FAILURE);
		}
		//wait(pid, NULL, 0);
	}
}
void parse_cmd(char* cmd){
	
	char** arg = (char **)malloc(sizeof(char **) * MAX_CMD_COUNT);
	int i = 0, q_flag = 0, j = 0;
	
	int cmd_index = 0, cmd_length = 0;
	for(j = 0;j < MAX_CMD_COUNT;j++){
		arg[j] = (char *)malloc(sizeof(char *) * MAX_CMD_LENGTH);
	}
	//printf("buffer: %lu\n", strlen(cmd));
	for(i = parse_index; i < strlen(cmd); i++){
		//printf("%d\n", i);
		if(cmd[i] == '>'){
			
			//parse file name, dup, exec
			if( (q_flag % 2) == 0){
				
				int s = 0, d;
				char target_file[128];
				
				
				for(d = i + 1; d < strlen(cmd); d++){
					if(cmd[d]!=' ' && cmd[d]!='\n')
						target_file[s++] = cmd[d];

				}
				target_file[s] = '\0';
				printf("target: %s, %d\n", target_file, cmd_index);
				int fd = open(target_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				dup2(fd, STDOUT_FILENO);

				i = strlen(cmd);
				int h = 0;
				for(h = cmd_index + 1; h < MAX_CMD_COUNT; h++){
					arg[h] = NULL;
				}
				send(connectfd, "\n", 1, 0);
				if(execvp(arg[0], arg) == -1){

					fprintf(stderr, "Error: Unable to load the executable \n");
            		exit(EXIT_FAILURE);
				
				}		
				break;		
			}
			
			
		}
		
		if(cmd[i] == '|'){
			printf("||||\n");
			if( (q_flag % 2) == 0){
				int h = 0;
				for(h = cmd_index + 1; h < MAX_CMD_COUNT; h++){
					arg[h] = NULL;
				}
				parse_index = i + 1;
				exec_cmd(arg);
				continue;
			}
		}

		if(cmd[i]!= ' '){
			//printf("not a space: %c\n", cmd[i]);
			if(cmd[i] == '\n' || cmd[i] == '\0' || i == strlen(cmd) - 1){
				//printf("arg: %s\n", arg[0]);
				int h = 0;
				for(h = cmd_index + 1; h < MAX_CMD_COUNT; h++){
					arg[h] = NULL;
				}
				dup2(connectfd, STDOUT_FILENO);
				if(execvp(arg[0], arg) == -1){
			
					fprintf(stderr, "Error : Unable to load the executable \n");
            		exit(EXIT_FAILURE);
				}
			}else{
				//printf("%s\n", "set arg" );
				arg[cmd_index][cmd_length] = cmd[i];
				cmd_length++;
				//printf("%c\n", arg[cmd_index][cmd_length-1]);
			}
		}else{
			
			if(cmd[i-1] != ' '){
				if(cmd[i+1] != '|' && cmd[i+1] != '>'){
					cmd_index++;
					cmd_length = 0;
				}
				
			}
		
		}

		if(cmd[i] == '\''){
			q_flag++;
		}
		
	}
}