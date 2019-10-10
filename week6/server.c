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

/* Driver function */
int main(int argc, char const *argv[])
{
    int sockfd, newSockfd;
    socklen_t len;
    int port;
    struct sockaddr_in servaddr, cli;
    char buffer[BUFFER_MAX_LEN];

    /* child pid for fork */
    pid_t pid;

    /* config server */
    if (argc < 2)
        die("Must provide a port number.\nUsage: ./server <PORT Number>\n", 1);

    port = getServerPort(argv[1]);

    if (port == -1)
        die("Invalid port number.\n", 1);

    /* socket create and verification */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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

    if ((listen(sockfd, 5)) != 0)
        die("Listen failed...\n", 1);
    else
        printf("Server listening..\n");

    while (1)
    {
        /* Accept the data packet from client and verification */
        if((newSockfd = accept(sockfd, (struct sockaddr *)&cli, &len)) < 0)
            die("server acccept failed...\n", 1);
        else
            printf("server acccepted the client %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

        if ((pid = fork()) == 0) {
            close(sockfd);
            while(1) {
                if(read(newSockfd, buffer, BUFFER_MAX_LEN) == -1)
                    die("read error", 1);
                printf("Client %s: %s", inet_ntoa(cli.sin_addr), buffer);
                if(write(newSockfd, buffer, BUFFER_MAX_LEN) == -1)
                    die("write error", 1);
                bzero(buffer, BUFFER_MAX_LEN);
            }
        }
        
    }
    /* close the socket */
    close(newSockfd);

    return 0;
}