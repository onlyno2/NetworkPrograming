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

#include "account.h"

#define BUFFER_MAX_LEN 1024

void die(char *msg, int type);
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

/* driver code */
int main(int argc, char const *argv[])
{
	int sockfd;
	int port;
	struct sockaddr_in servaddr;
	char buffer[BUFFER_MAX_LEN];
	int logged_in = 0;

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

	int counter = 0;
	int n;

loop:
	do
	{
		printf("\n\n1. login\n2. logout\nEnter your choice: ");
		scanf("%d%*c", &n);
		switch (n)
		{
		case 1:
			printf("logged_in: %d\n", logged_in);
			if (!logged_in)
			{
				goto A;
			}
			else
			{
				printf("Logged in. cannot login\n");
			}
			break;
		case 2:
			printf("logged_in: %d\n", logged_in);
			if (logged_in)
			{
				printf("logged out\n");
				logged_in = 0;
			}
			else
			{
				printf("not log in\n");
			}
			break;
		default:
			printf("invalid choice\n");
			exit(1);
			break;
		}
	} while (1);

A:
	printf("Username: ");
	scanf("%s%*c", buffer);
	if (write(sockfd, buffer, BUFFER_MAX_LEN) == -1)
		die("write error", 1);

	if ((n = read(sockfd, buffer, BUFFER_MAX_LEN)) < 0)
		printf("[-]Error in receiving data.\n");
	else
		buffer[n] = '\0';

	/* handle server return message */
	if (strcmp(buffer, "User does not exist!!") == 0 || strcmp(buffer, "User blocked!!") == 0)
		die(buffer, 0);

	if (strcmp(buffer, "Valid user") != 0)
		die(buffer, 0);

	do
	{
		bzero(buffer, BUFFER_MAX_LEN);
		/* input password */
		if (counter == 0)
		{
			printf("Password: ");
			scanf("%s%*c", buffer);
		}
		else
		{
			printf("Password (wrong %d time(s)): ", counter);
			scanf("%s%*c", buffer);
		}
		/* send username */
		if (write(sockfd, buffer, BUFFER_MAX_LEN) == -1)
			die("write error", 1);
		/* receive password */
		if ((n = read(sockfd, buffer, BUFFER_MAX_LEN)) < 0)
			printf("[-]Error in receiving data.\n");
		else
			buffer[n] = '\0';

		/* if match password */
		if (strcmp(buffer, "Logged in") == 0)
		{
			printf("logged in\n");
			break;
		}

		printf("%s\n", buffer);
		counter++;
	} while (counter < 3);

	/* not match over 3 times */
	if (counter == 3)
		die("Password not match 3 times!! User has been blocked", 0);
	bzero(buffer, BUFFER_MAX_LEN);

	logged_in = 1;
	goto loop;
}