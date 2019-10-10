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

/* driver code */
int main(int argc, char const *argv[])
{
	int sockfd;
	int port;
	struct sockaddr_in servaddr;
    char buffer[BUFFER_MAX_LEN];

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

    while(1) {
        printf("Client: ");
        scanf("%s%*c", buffer);
        if(write(sockfd, buffer, strlen(buffer)) == -1)
            die("write error", 1);
        if(read(sockfd, buffer, BUFFER_MAX_LEN) < 0)
            die("read error", 1);

        printf("Server: %s\n", buffer);
        bzero(buffer, BUFFER_MAX_LEN);
    }

    /* close the socket */
	close(sockfd);
}