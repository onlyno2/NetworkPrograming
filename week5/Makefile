CC = gcc
CFLAGS = -c -Wall

all: client.o server.o
	$(CC) -o client client.o
	$(CC) -o server server.o

client.o: client.c
	$(CC) $(CFLAGS) client.c
	$(CC) $(CFLAGS) server.c