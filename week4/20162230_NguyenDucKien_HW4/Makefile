CC = gcc
CFLAGS = -c -Wall

all: server.o client.o
	$(CC) -o client client.o
	$(CC) -o server server.o

server.o: server.c
	$(CC) $(CFLAGS) server.c

client.o: client.c
	$(CC) $(CFLAGS) client.c


clean: 
	rm *.o server client