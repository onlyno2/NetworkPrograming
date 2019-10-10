<<<<<<< HEAD
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
    @param port number from cli argument

    @return port number || -1 if invalid

    get server port from command line
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
    @param message type 1 for socket, 0 for none

    stop program with message
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
    @param buffer from client

    @return buffer_out_char: string contain only character
    @return buffer_out_digit: string contain only digit

    create character string and digit string from buffer
*/
int handleBuffer(char *buffer, char *buffer_out_char, char *buffer_out_digit)
{
    int buffer_char_counter = 0, buffer_digit_counter = 0;
    for (int i = 0; i < strlen(buffer); i++)
    {
        /* if character */
        if (isalpha(buffer[i]))
        {
            buffer_out_char[buffer_char_counter++] = buffer[i];
        }
        /* if digit */
        else if (isdigit(buffer[i]))
        {
            buffer_out_digit[buffer_digit_counter++] = buffer[i];
        }
        /* invalid character */
        else
        {
            return -1;
        }
    }
    buffer_out_char[buffer_char_counter] = '\0';
    buffer_out_digit[buffer_digit_counter] = '\0';
    return 1;
}


/* 
    @param socket file descriptor

    handle message string sent by client
*/
void handleString(int sockfd)
{
    char buffer[BUFFER_MAX_LEN];
    char buffer_out_char[BUFFER_MAX_LEN], buffer_out_digit[BUFFER_MAX_LEN];
    int n;

    while (1)
    {
        bzero(buffer, BUFFER_MAX_LEN);
        /* read message from client and copy it in buffer */
        if ((n = read(sockfd, buffer, sizeof(buffer))) == -1)
            die("read error", 1);
        if (n == 0)
        {
            printf("client disconnected\n");
            break;
        }

        buffer[n] = '\0';
        printf("client message: %s\n", buffer);
        /* if buffer contain invalid character */
        if (handleBuffer(buffer, buffer_out_char, buffer_out_digit) == -1)
        {
            strcpy(buffer, "Invalid character");
            if (write(sockfd, buffer, sizeof(buffer)) == -1)
                die("write error", 1);
        }
        /* if valid buffer */
        else
        {
            if (write(sockfd, buffer_out_char, sizeof(buffer_out_char)) == -1)
                die("write error", 1);

            if (write(sockfd, buffer_out_digit, sizeof(buffer_out_digit)) == -1)
                die("write error", 1);
        }
    }
}


/*
    @param socket file descriptor

    handle data read from file sent by client
*/
void handleFile(int sockfd)
{
    char buffer[BUFFER_MAX_LEN];
    int n;
    while (1)
    {
        bzero(buffer, BUFFER_MAX_LEN);
        if ((n = read(sockfd, buffer, sizeof(buffer))) == -1)
            die("read error", 1);
        if (n == 0)
            break;

        /* if message exit */
        if (strcmp(buffer, "exit") == 0)
        {
            printf("end\n");
            break;
        }
        /* if normal message */
        else
            printf("%s", buffer);
    }
}

/*
    @param socket file descriptor

    get client message send mode and handle
*/
int getClientData(int sockfd)
{
    char buffer[BUFFER_MAX_LEN];
    bzero(buffer, BUFFER_MAX_LEN);
    printf("waiting for client data ....\n");
    /* read message from client and copy it in buffer */
    int n;
    n = read(sockfd, buffer, sizeof(buffer));
    /* if read error */
    if (n == -1)
        die("[] read error", 1);
    /* if empty message */
    if (n == 0)
        return 1;
    printf("%s\n", buffer);
    /* check running mode: string or file */
    if (strcmp(buffer, "file") == 0)
        handleFile(sockfd);
    else if (strcmp(buffer, "string") == 0)
        handleString(sockfd);
    else return 0;

    return 1;
}

/* Driver function */
int main(int argc, char const *argv[])
{
    int sockfd, connfd;
    socklen_t len;
    int port;
    struct sockaddr_in servaddr, cli;

    /* config server */
    if (argc < 2)
        die("Must provide a port number.\nUsage: ./server <PORT Number>\n", 1);

    port = getServerPort(argv[1]);

    if (port == -1)
        die("Invalid port number.\n", 1);

    /* socket create and verification */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        die("socket creation failed...\n", 1);
    else
        printf("Socket successfully created..\n");

    /* empty server to config */
    bzero(&servaddr, sizeof(servaddr));

    /* assign IP, PORT */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = port;

    /* Binding newly created socket to given IP and verification */
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
        die("socket bind failed...\n", 1);
    else
        printf("Socket successfully binded..\n");

    /* Now server is ready to listen and verification */
    while (1)
    {
        /* listen for client */
        if ((listen(sockfd, 5)) != 0)
            die("Listen failed...\n", 1);
        else
            printf("Server listening..\n");
        len = sizeof(cli);

        /* Accept the data packet from client and verification */
        connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
        if (connfd < 0)
            die("server acccept failed...\n", 1);
        else
            printf("server acccept the client...\n");

        /* get client data */
        if (getClientData(connfd))
            continue;
    }
    /* close the socket */
    close(sockfd);
    return 0;
}
=======
#include <stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#define MAX 80 
#define PORT 8080 

/*
    @param: message to inform

    Inform message and exit program if error 
*/
void die(const char* message) 
{
    perror(message);
    exit(1);
}

void func(int sockfd) 
{ 
	char buff[MAX]; 
	int n; 
	// infinite loop for chat 
	for (;;) { 
		bzero(buff, MAX); 

		// read the message from client and copy it in buffer 
		read(sockfd, buff, sizeof(buff)); 
		// print buffer which contains the client contents 
		printf("From client: %s\t To client : ", buff); 
		bzero(buff, MAX); 
		n = 0; 
		// copy server message in the buffer 
		while ((buff[n++] = getchar()) != '\n') 
			; 

		// and send that buffer to client 
		write(sockfd, buff, sizeof(buff)); 

		// if msg contains "Exit" then server exit and chat ended. 
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		} 
	} 
} 

// Driver function 
int main() 
{ 
	int sockfd, connfd;
    socklen_t len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n");

	// Function for chatting between client and server 
	func(connfd); 

	// After chatting close the socket 
	close(sockfd); 
} 
>>>>>>> 3d41c5069334180b30c0c0d0dea6b39eb457e1dc
