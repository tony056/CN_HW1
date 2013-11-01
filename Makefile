all: client realServer
client: client.c
	gcc -Wall -o rsh-client client.c -pthread
realServer: realServer.c
	gcc -Wall -o rsh-server realServer.c
clean:
	rm -rf rsh-client rsh-server
