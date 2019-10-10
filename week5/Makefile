CC = gcc
CFLAGS = -c -Wall

<<<<<<< HEAD
all: server.o client.o
	$(CC) -o client client.o
	$(CC) -o server server.o

server.o: server.c
	$(CC) $(CFLAGS) server.c

client.o: client.c
	$(CC) $(CFLAGS) client.c


clean: 
	rm *.o server client
=======
all: client.o server.o
	$(CC) -o client client.o
	$(CC) -o server server.o

client.o: client.c
	$(CC) $(CFLAGS) client.c
	$(CC) $(CFLAGS) server.c
>>>>>>> 3d41c5069334180b30c0c0d0dea6b39eb457e1dc
