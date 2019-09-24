CFLAGS = -c -Wall
CC = gcc

all: resolver

resolver: main.o net.o
	$(CC) -o resolver main.o net.o
main.o: main.c
	$(CC) $(CFLAGS) main.c

net.o: net.c
	$(CC) $(CFLAGS) net.c

clean:
	rm -f *.o resolver