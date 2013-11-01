#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

/* define area */ 
#define PORT_NUM (8001)
#define MAX_CMD_COUNT (15)
#define MAX_CMD_LENGTH (100)
#define MAX_LENGTH (9999)

/* global variable */
int listenfd, connectfd;
socklen_t length;
struct sockaddr_in serverAddress, clientAddress;
int pipe_length = 0;
int arg_range = 0;
int advance = 0;
int parse_index = 0;
int parse = 0;
char* buffer;
int wpipe_fd[2];
int rpipe_fd[2];
int write_flag = 0;

char* waiting_cmd;

/* function */
void setServer();
void buildSocketConnect();
void setEnv();
void exec_shell();
void exec_cmd();
void parse_cmd(char *str);
int check_setEnv(char* cmd);
int check_printEnv(char* cmd);
int check_wait_num();

/* structure of advanced pipe */
typedef struct ns
{
	int waiting_num;
	char command[MAX_LENGTH];
	struct ns* next;
} node;

/* function for linked list */
void create_node(int num, char* arg);
void insert_node(node* next);
char* remove_node();
node* first = NULL;



int main(int argc, char *argv[]){

	pid_t pid;
	buildSocketConnect();

	printf("%s\n","Server running...waiting for connections.");

	buffer = (char *)malloc(sizeof(char) * MAX_LENGTH);
	bzero(&buffer, sizeof(buffer));
	char* initPath = "bin:.";
	setEnv(initPath);
	waiting_cmd = (char *)malloc(sizeof(char) * MAX_LENGTH);

	while(1){
		
		length = sizeof(clientAddress);

		connectfd = accept (listenfd, (struct sockaddr *) &clientAddress, &length);
        printf("%s\n","Received request...");
 		write(connectfd, "#########################\n", 26);
 		write(connectfd, "##     Welcome         ##\n", 26);
 		write(connectfd, "##     b00902056's     ##\n", 26);
 		write(connectfd, "##     shell           ##\n", 26);
 		write(connectfd, "#########################\n", 26);

        if((pid = fork()) < 0 ){
        	printf("server fork error\n");
        	return 0;
        	//error
        }else if(pid == 0){
        	//child
        	dup2(connectfd, STDERR_FILENO);
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
	int n = 0, cmd_pid, b = 0, c = 0;
	char* set_path;
	set_path = (char *)malloc(sizeof(char) * MAX_CMD_LENGTH);
	while((n = recv(connectfd, buffer, MAX_LENGTH, 0)) > 0){
		//fprintf(stderr, "%s\n", buffer);
		write_flag = check_wait_num();
		//fprintf(stderr, "write_flag: %d\n", write_flag);
		if(strncmp(buffer, "setenv PATH", 11) == 0){
			//setenv
			for(b = 11; b < strlen(buffer); b++){
				if(buffer[b]!= ' ' && buffer[b]!= '\n'){
					set_path[c++] = buffer[b];
				}
			}
			set_path[c] = '\0';
			setenv("PATH", set_path, 1);
		}else{

				if(pipe(wpipe_fd) == -1){
		
					//fprintf(stderr, "Error: Unable to create pipe. \n");
			        exit(EXIT_FAILURE);
				}
				if(pipe(rpipe_fd) == -1){

					//fprintf(stderr, "Error: Unable to create pipe. \n");
			        exit(EXIT_FAILURE);	
				}
				cmd_pid = fork();
				if(cmd_pid < 0){
					//printf("server fork error\n");
		        	exit(EXIT_FAILURE);
				}else if(cmd_pid == 0){
					dup2(wpipe_fd[0], STDIN_FILENO);
					close(wpipe_fd[0]);
					close(wpipe_fd[1]);
					close(rpipe_fd[0]);
					//dup2(wpipes_fd[])
					parse_cmd(buffer);	
				}else{
				
					//dup2(wpipe_fd[1], STDOUT_FILENO);
					//close(wpipe_fd[1]);
					close(wpipe_fd[0]);
					close(rpipe_fd[1]);
					//close(wpipe_fd[0]);
					if(write_flag != 0){
						char* output = remove_node();
						
						write(wpipe_fd[1], output, sizeof(char) * strlen(output));
						//fprintf(stderr, "fuck you ass hole: %zd\n", );
						close(wpipe_fd[1]);
					}
					int num = -1;
					char *wait_outcome = (char*)malloc(sizeof(char) * MAX_LENGTH);
					read(rpipe_fd[0], &num, sizeof(int));
					//fprintf(stderr, "wpipe_fd[0]: %d\n", num);
					read(rpipe_fd[0], wait_outcome, sizeof(char) * MAX_LENGTH);
					//fprintf(stderr, "wait_outcome: %s\n", wait_outcome);
					if(num > 0){
						create_node(num, wait_outcome);
					}

					//printf("child: %dup2\n", ++child);
				}

		}                    
		memset(buffer, '\0', sizeof(char) * n);
		//check wait num function: for loop to --
		
		
		
	}
			 
}
void exec_cmd(char** arg){
	parse++;
	int pipes_fd[2];

	
	if(arg == NULL){
		//printf("%s\n", "arg is NULL");
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
		
		//printf("%d %s\n", pid, buffer);
		dup2(pipes_fd[0], STDIN_FILENO);
		dup2(pipes_fd[1], STDOUT_FILENO);
		close(pipes_fd[1]);
		close(pipes_fd[0]);
		//child
		parse_cmd(buffer);
		exit(0);
		
	}else{

		close(pipes_fd[0]);
		dup2(pipes_fd[1], STDOUT_FILENO);
		//printf("arg[1]:%s\n", arg[1]);
		if(execvp(arg[0], arg) == -1){
			
			fprintf(stderr, "parent: Error: Unable to load the executable \n");
            exit(EXIT_FAILURE);
		}
		//wait(pid, NULL, 0);
	}
}
void parse_cmd(char* cmd){
	//printf("---parse_index: %d\n", parse_index);
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
				//printf("target: %s, %d\n", target_file, cmd_index);
				int fd = open(target_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				dup2(fd, STDOUT_FILENO);

				i = strlen(cmd);
				int h = 0;
				for(h = cmd_index + 1; h < MAX_CMD_COUNT; h++){
					arg[h] = NULL;
				}
				//send(connectfd, "ok", 2, 0);
				if(check_printEnv(arg[0]) > 0 && strcmp(arg[1], "PATH") == 0){
								//printf("arg[1] = %s",arg[1]);
								//printf("------\n");
								
					printf("PATH=%s\n", getenv(arg[1]));
								//send(connectfd, path, strlen(path), 0);
				}else{
					if(execvp(arg[0], arg) == -1){

						fprintf(stderr, "Error: Unable to load the executable \n");
	            		exit(EXIT_FAILURE);
					
					}
				}		
				break;

			}
			
			
		}
		
		if(cmd[i] == '|'){
			//printf("||||\n");
			if( (q_flag % 2) == 0){
				//printf("||||\n");
				int h = 0;
				for(h = cmd_index + 1; h < MAX_CMD_COUNT; h++){
					arg[h] = NULL;
				}
				int a = 0;
				for (a = i + 1; a < strlen(cmd); a++)
				{
					if(cmd[a] != ' '){
						break;
					}
				}
				//printf("aaaa: %d\n", a);
				parse_index = a;
				//printf("parse_index:%d\n", parse_index);
				//printf("arg[0] = %s,,,arg[1] = %s\n", arg[0], arg[2]);
				exec_cmd(arg);
				//continue;
				break;
			}
		}

		if(cmd[i]!= ' '){
			//printf("not a space: %c\n", cmd[i]);
			if(cmd[i] == '\''){
				//printf("%d\n", q_flag);
				q_flag++;
				continue;
			}
			if(cmd[i] == '\n' || cmd[i] == '\0' || i == strlen(cmd) - 1){
				//printf("arg: %s\n", arg[1]);
				if(q_flag % 2 == 0){
					
					int h = 0;
					for(h = cmd_index + 1; h < MAX_CMD_COUNT; h++){
						arg[h] = NULL;
					}
					if(i!= 0){
						
						if(isdigit(arg[cmd_index][0])){
							//fprintf(stderr, "isdigit\n");
							int a = 0;
							char chnum[4];
							for(a = 0; a < strlen(arg[cmd_index]); a++){
								chnum[a] = arg[cmd_index][a];
							}
							chnum[strlen(arg[cmd_index])] = '\0';
							int num = atoi(chnum);
							//read from stdin
							//fprintf(stderr, "nummmmm: %d\n", num);
							read(STDIN_FILENO, waiting_cmd, MAX_LENGTH);
							//fprintf(stderr, "waiting_cmd: %s\n", waiting_cmd);
							//create_node(num, waiting_cmd);
							write(rpipe_fd[1], &num, sizeof(int));//write to another pipe.
							write(rpipe_fd[1], waiting_cmd, MAX_LENGTH);
							break;
						}else{	
							
							dup2(connectfd, STDOUT_FILENO);
							close(connectfd);
							// int nosend = -1;
							// char* nomessage = "fuck";
							// write(wpipe_fd[1], &nosend, sizeof(int));//write to another pipe.
							// write(wpipe_fd[1], nomessage, sizeof(char) * strlen(nomessage));
							if(check_printEnv(arg[0]) > 0 && strcmp(arg[1], "PATH") == 0){
								//printf("arg[1] = %s",arg[1]);
								//printf("------\n");
								
								printf("PATH=%s\n", getenv(arg[1]));
								//send(connectfd, path, strlen(path), 0);

							}else{
								
								if(execvp(arg[0], arg) == -1){
							
									fprintf(stderr, "Error : Unable to load the executable \n");
				            		exit(EXIT_FAILURE);
								}

							}
						}
						
					}
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

		
		
	}
}
int check_setEnv(char* cmd){
	if(strcmp(cmd, "setenv") != 0){
		return -1;
	}
	return 1;
}
int check_printEnv(char* cmd){
	if(strcmp(cmd, "printenv") == 0){
		return 1;
	}
	return -1;	
}

void create_node(int num, char* arg){
	
	if(first != NULL){

		node* adpipe = (node*)malloc(sizeof(node));

		adpipe->waiting_num = num;
		strncpy(adpipe->command, arg, strlen(arg));
		adpipe->next = NULL;
		//adpipe->command[strlen(arg)] = '\0';
		insert_node(adpipe);
	}else{

		first = (node*)malloc(sizeof(node));

		first->waiting_num = num;
		strncpy(first->command, arg, strlen(arg));
		first->next = NULL;
		//fprintf(stderr, "command : %s\n",first->command);

	}

}

void insert_node(node* next){
	node* ptr = first;
	while(ptr->next != NULL){
		ptr = ptr->next;
	}

	ptr->next = next;
	
}

char* remove_node(){
	char* out = (char* )malloc(sizeof(char) * MAX_LENGTH);
	memset(out, '\0', sizeof(char) * MAX_LENGTH);
	node* ptr = first;
	node* prev = first;
	while(ptr != NULL){
		if(ptr->waiting_num == 0){
			if(strlen(out) == 0){
				strcpy(out, ptr->command);
			}else{
				strcat(out, ptr->command);
			}
		}
		prev = ptr;
		
		if(ptr == first){
			first = NULL;
			//fprintf(stderr, "%s\n", "first node is null");
		}
		ptr = ptr->next;
		free(prev);
	}
	//fprintf(stderr, "remove: %s\n", out);
	return out;
}

int check_wait_num(){
	
	node* ptr = first;
	int wait_node = 0;
	while(ptr != NULL){
		// fprintf(stderr, "%s\n", "check check");
		// fprintf(stderr, "ptr->waiting_num: %d\n", ptr->waiting_num);
		// fprintf(stderr, "ptr->command: %s\n", ptr->command);
		if(ptr->waiting_num > 0)
			ptr->waiting_num--;
		if(ptr->waiting_num == 0){
			wait_node++;
		}
		ptr = ptr->next;

	}
	
	return wait_node;
}