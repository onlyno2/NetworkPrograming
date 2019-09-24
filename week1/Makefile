CFLAGS = -c -Wall
CC = gcc

all: main

main: main.o user.o

main.o: main.c
	$(CC) $(CFLAGS) main.c

user.o: user.c
	$(CC) $(CFLAGS) user.c

clean:
	rm -f *.o main account.txt