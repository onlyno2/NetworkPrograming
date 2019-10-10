#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_MAX_LEN 1024

/*
    @param port in string

    return port number if valid port or -1 if invalid
*/
int getServerPort(const char *port_argument)
{
	int port_num = atoi(port_argument);

	if (port_num > 0 && port_num < 65535)
		return port_num;
	else
		return -1;
}

/*
    @param message
    @param type

    exit program when error encounter or get exit message
*/
void die(char *msg, int type)
{
	if (type == 1)
	{
		perror(msg);
	}
	else if (type == 0)
	{
		printf("%s\n", msg);
	}
	exit(1);
}

/*
	@param socket file descriptor

	send strings to server
*/
void sendString(int sockfd)
{
	char buffer[BUFFER_MAX_LEN];
	/* send mode */
	if (write(sockfd, "string", strlen("string")) == -1)
		die("write error", 1);

	/* input loop */
	while (1)
	{
		/* reset buffer */
		bzero(buffer, BUFFER_MAX_LEN);
		/* input message */
		printf("Enter message: ");
		gets(buffer);

		/* exit if input null */
		if (buffer[0] == '\0' || buffer[0] == '\n')
		{
			// if (write(sockfd, "exit", strlen("exit")) == -1)
			// 	die("write error", 1);
			die("exit", 0);
		}

		/* send message */
		if (write(sockfd, buffer, sizeof(buffer)) == -1)
			die("write error", 1);

		/* read return string buffer */
		bzero(buffer, BUFFER_MAX_LEN);
		if (read(sockfd, buffer, BUFFER_MAX_LEN) == -1)
			die("read error", 1);
		printf("%s\n", buffer);

		/* if invalid character encountered */
		if (strcmp(buffer, "Invalid character") == 0)
			continue;

		/* read return digit buffer */
		bzero(buffer, BUFFER_MAX_LEN);
		if (read(sockfd, buffer, BUFFER_MAX_LEN) == -1)
			die("read error", 1);
		printf("%s\n", buffer);
	}
}

/*
	@param socket file descriptor

	send file content to server
*/
void sendFile(int sockfd)
{
	char buffer[BUFFER_MAX_LEN];
	FILE *file;
	/* reset buffer */
	bzero(buffer, BUFFER_MAX_LEN);

	/* send mode */
	if (write(sockfd, "file", strlen("file")) == -1)
		die("write error", 1);

	/* input file name */
	printf("Input file name: ");
	scanf("%s%*c", buffer);

	/* open file */
	file = fopen(buffer, "r");
	/* open error */
	if (file == NULL)
		die("open file error ", 0);

	/* file loop */
	while (!feof(file))
	{
		/* reset buffer */
		bzero(buffer, BUFFER_MAX_LEN);
		/* read a line from file */
		fgets(buffer, BUFFER_MAX_LEN, file);
		/* send message */
		if (write(sockfd, buffer, sizeof(buffer)) == -1)
			die("write error", 1);
	}
	/* end of file  */
	if (write(sockfd, "exit", strlen("exit")) == -1)
		die("write error", 1);

	fclose(file);
}

/* driver code */
int main(int argc, char const *argv[])
{
	int sockfd;
	int port;
	struct sockaddr_in servaddr;

	/* config client */
	if (argc < 3)
		die("Must provide a port number.\nUsage: ./client <Server Address> <PORT Number>\n", 0);

	if (argc < 2)
		die("Must provide a server address.\nUsage: ./client <Server Address> <PORT Number>\n", 0);

	port = getServerPort(argv[2]);

	if (port == -1)
		die("Invalid port number.\n", 1);

	/* socket create and varification */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		die("socket creation failed", 1);
	else
		printf("Socket successfully created..\n");

	/* reset server for config */
	bzero(&servaddr, sizeof(servaddr));

	/* assign IP, PORT */
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = port;

	/* connect the client socket to server socket */
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
		die("connection failed", 1);
	else
		printf("connected to the server..\n");
	/* mode choose */
	int choice;
	printf("\n1. Send strings\n2. Send text file content\n");
	printf("Choose option: ");
	scanf("%d%*c", &choice);

	switch (choice)
	{
	case 1:
		sendString(sockfd);
		break;
	case 2:
		sendFile(sockfd);
		break;
	default:
		die("invalid option", 0);
	}

	/* close the socket */
	close(sockfd);
}