all: server client realServer
server: server.c
	gcc -Wall -o server server.c
client: client.c
	gcc -Wall -o client client.c
realServer: realServer.c
	gcc -Wall -o realServer realServer.c
clean:
	rm -rf server client realServer
