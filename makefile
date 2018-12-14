all: server.c client.c
	gcc -pthread -o server server.c
	gcc -pthread -o client client.c
clean:
	rm server
	rm client
