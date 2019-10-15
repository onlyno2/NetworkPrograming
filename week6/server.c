#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#include "account.h"

AccountNode *accountList;

#define BUFFER_MAX_LEN 1024 /* max number of bytes can get at once */

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
    _exit(1);
}

/*
	read data from account.txt
*/
void readData(void)
{
    FILE *file = fopen("account.txt", "r");
    char username[MAX_ACCOUNT_LEN], password[MAX_PASSWORD_LEN];
    int status;
    if (file == NULL)
        die("read data error", 0);

    while (!feof(file))
    {
        bzero(username, sizeof(username));
        bzero(password, sizeof(password));
        fscanf(file, "%s %s %d", username, password, &status);
        printf("%s - %s - %d\n", username, password, status);
        insertAccountNode(&accountList, createAccountNode(username, password, status));
    }
    fclose(file);
}

/*
    write data to account.txt
*/
void writeData(void)
{
    FILE *file = fopen("account.txt", "w");
    if (file == NULL)
        die("write data error", 0);

    AccountNode *tmp = accountList;
    while (tmp)
    {
        fprintf(file, "%s %s %d\n", tmp->username, tmp->password, tmp->status);
        tmp = tmp->next;
    }

    fclose(file);
}

/*
    @param socket file descriptor

    handle request from client
*/
void handleRequest(int connfd)
{
    char buffer[BUFFER_MAX_LEN];
    AccountNode *user;

    int n;
    bzero(buffer, BUFFER_MAX_LEN);
    /* read username */
    if ((n = read(connfd, buffer, BUFFER_MAX_LEN)) == -1)
        die("read error", 1);
    buffer[n] = '\0';

    /* get user info from username */
    user = searchAccountNode(accountList, buffer);

    /* if user not exist */
    if (user == NULL)
    {
        if (write(connfd, "User does not exist!!", strlen("User does not exist!!")) == -1)
            die("write error", 1);
        die("user not exist", 0);
    }
    /* if blocked user */
    if (user->status == 0)
    {
        if (write(connfd, "User blocked!!", strlen("User blocked!!")) == -1)
            die("write error", 1);
        die("User blocked!!", 0);
    }

    /* valid user */
    if (write(connfd, "Valid user", strlen("Valid user")) == -1)
        die("write error", 1);

    int counter = 0;
    bzero(buffer, BUFFER_MAX_LEN);
    do
    {
        bzero(buffer, BUFFER_MAX_LEN);
        /* read password */
        if ((n = read(connfd, buffer, BUFFER_MAX_LEN)) == -1)
            die("read error", 1);
        else
            buffer[n] = '\0';
        /* match password */
        if (strcmp(buffer, user->password) == 0)
        {
            if (write(connfd, "Logged in", strlen("Logged in")) == -1)
                die("write error", 1);
            break;
        }
        /* password not match */
        if (write(connfd, "Password not match", strlen("Password not match")) == -1)
            die("write error", 1);
        counter++;
    } while (counter < 3);

    /* password not match over 3 times */
    if (counter == 3)
    {
        user->status = 0;
        /* write data to account.txt and quit handle */
        writeData();
        _exit(0);
    }

    /* read disconnect message */
    if (n = read(connfd, buffer, BUFFER_MAX_LEN) == -1)
        die("read error", 1);
}

/* Driver function */
int main(int argc, char const *argv[])
{
    int sockfd, newSocket;
    int port;
    int num_bytes;
    struct sockaddr_in servaddr;
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    char buffer[BUFFER_MAX_LEN];

    /* child pid for fork */
    pid_t pid;

    /* read data from account.txt */
    readData();

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
        /* accept client */
        newSocket = accept(sockfd, (struct sockaddr *)&cli, &len);
        if (newSocket < 0)
        {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

        /* create new child pid */
        if ((pid = fork()) == 0)
        {
            close(sockfd);
            /* handle message from client */
            handleRequest(newSocket);

            /* disconnect after logged in */
            printf("disconnected with %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
            exit(0);
        }
    }

    close(newSocket);
    return 0;
}
