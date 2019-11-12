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
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

#include "account.h"

AccountNode *accountList;

/* max number of bytes can get at once */
#define BUFFER_MAX_LEN 1024
#define MAX_CLIENT 30

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
        printf("user not exist");
        goto end;
    }
    /* if blocked user */
    else if (user->status == 0)
    {
        if (write(connfd, "User blocked!!", strlen("User blocked!!")) == -1)
            die("write error", 1);
        printf("User blocked!!");
        goto end;
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
        /* write data to account.txt */
        writeData();
        goto end;
    }

    end: printf("end handling message\n");
}

/* Driver function */
int main(int argc, char const *argv[])
{
    /* parent socket */
    int parentfd;
    /* child socket */
    int childfd;
    socklen_t clientlen;
    const int on = 1;
    /* server's addr */
    struct sockaddr_in serveraddr;
    /* client addr */
    struct sockaddr_in clientaddr;
    /* flag value for setsockopt */
    int optval;
    /* message byte size */
    int n;
    /* max fd set */
    int maxfd;
    /* file descriptor set */
    fd_set master_set;
    /* time val */
    struct timeval tv;
    /* client fd info */
    int client_socket[MAX_CLIENT];
    char buffer[BUFFER_MAX_LEN];

    int port;

    /* config server */
    if (argc < 2)
        die("Must provide a port number.\nUsage: ./server <PORT Number>\n", 1);

    port = getServerPort(argv[1]);

    if (port == -1)
        die("Invalid port number.\n", 1);

    /* Initialize all client_socket[] to zero so not checked */
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        client_socket[i] = 0;
    }

    /* socket create and verification */
    if ((parentfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        die("socket creation failed...\n", 1);
    else
        printf("Socket successfully created..\n");

    /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
    optval = 1;
    if (setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval, sizeof(int)) < 0)
    {
        die("set sock opt", 1);
    }
    /* empty server to config */
    bzero(&serveraddr, sizeof(serveraddr));
    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;
    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* this is the port we will listen on */
    serveraddr.sin_port = port;

    /* Binding newly created socket to given IP and verification */
    if ((bind(parentfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
        die("socket bind failed...\n", 1);
    else
        printf("Socket successfully binded..\n");

    /* read data from account.txt */
    readData();

    /* listen: make this socket ready to accept connection requests */
    if ((listen(parentfd, 5)) != 0)
        die("Listen failed...\n", 1);
    else
        printf("Server listening..\n");

    /* setup time out */
    tv.tv_sec = 3 * 60;
    tv.tv_usec = 0;
    int selectStatus;
    do
    {
        /* clear fdset */
        FD_ZERO(&master_set);
        /* add parentfd to set */
        FD_SET(parentfd, &master_set);
        maxfd = parentfd;

        /* add child socket to parent */
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            childfd = client_socket[i];
            /* add childfd to fdset */
            if (childfd > 0)
                FD_SET(childfd, &master_set);
            /* set maxfd */
            if (childfd > maxfd)
                maxfd = childfd;
        }

        /* wait activity on one fd */
        selectStatus = select(maxfd + 1, &master_set, NULL, NULL, &tv);

        if ((selectStatus < 0) && (errno != EINTR))
        {
            die("select error", 1);
        }

        if (FD_ISSET(parentfd, &master_set))
        {
            bzero(&clientaddr, sizeof(clientaddr));
            clientlen = sizeof(clientaddr);
            if ((childfd = accept(parentfd, (struct sockaddr *)&clientaddr, &clientlen)) == -1)
                die("accept error", 1);

            printf("new connection on fd: %d, client info: %s : %d\n",
                   childfd, inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);

            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = childfd;
                    break;
                }
            }
        }
        else
        {
            for (int i = 0; i < MAX_CLIENT; i++)
            {
                if (FD_ISSET(client_socket[i], &master_set))
                {
                    handleRequest(client_socket[i]);
                    close(client_socket[i]);
                    client_socket[i] = 0;
                }
            }
        }
    } while (1);

    close(parentfd);
    writeData();
    return 0;
}
