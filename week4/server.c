// server program for udp connection
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 1000
#define SOCKET_ERR 1
#define ERR 0

struct sockaddr_in cli1addr, cli2addr;

int compareClient(struct sockaddr_in cli1addr, struct sockaddr_in cli2addr);

void err_message(const char *message, int flag)
{
    if (flag == 1)
    {
        perror(message);
        exit(1);
    }
    if (flag == 0)
    {
        printf("%s", message);
        exit(1);
    }
}

int getPortNumber(const char *port_param)
{
    int port = atoi(port_param);

    if (port > 0 && port < 65535)
        return port;
    else
        return -1;
}

void getClientInfo(int sockfd)
{
    socklen_t len;
    char buffer[MAXLINE];
    struct sockaddr_in tmp;

    // first client addr
    printf("Getting client 1 info....\n");
    int n;
    if((n = recvfrom(sockfd, buffer, sizeof(buffer),
                     0, (struct sockaddr *)&tmp, &len)) == -1) 
        {
            err_message("Recv Error", SOCKET_ERR);
        }
    buffer[n] = '\0';
    cli1addr = tmp;
    
    printf("Getted client 1 information\n");
    printf("Getting client 2 info....\n");
    do {
        if((n = recvfrom(sockfd, buffer, sizeof(buffer),
                     0, (struct sockaddr *)&tmp, &len)) == -1) 
        {
            err_message("Recv Error", SOCKET_ERR);
        }
        if(compareClient(tmp, cli1addr) == 1) {
            continue;
        }
        else {
            cli2addr = tmp;
            break;
        }
    } while(1);
    printf("Getted client 2 information\n");
}

int compareClient(struct sockaddr_in cli1addr, struct sockaddr_in cli2addr) {
    if((cli1addr.sin_addr.s_addr == cli2addr.sin_addr.s_addr) && (cli1addr.sin_port == cli2addr.sin_port))
        return 1;
    else return 0;
}

// Driver code
int main(int argc, char const *argv[])
{
    char buffer[100];
    char *message = "Hello Client";
    int sockfd;
    int port;
    socklen_t len;
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    if (argc < 2)
    {
        err_message("Too few argument. Usage: ./server [port number]\n", 0);
    }

    if ((port = getPortNumber(argv[1])) == -1)
    {
        err_message("Invalid port number", 0);
    }

    // Create a UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        err_message("Sock error", 1);
    }
    printf("%d\n", port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    servaddr.sin_family = AF_INET;

    // bind server address to socket descriptor
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        err_message("Bind error", 1);
    }

    getClientInfo(sockfd);

    sendto(sockfd, "Connected", strlen("Connected"), 0, (struct sockaddr*)&cli1addr, sizeof(cli1addr)); 
    sendto(sockfd, "Connected", strlen("Connected"), 0, (struct sockaddr*)&cli2addr, sizeof(cli2addr)); 
    // send the response
    sendto(sockfd, message, MAXLINE, 0,
           (struct sockaddr *)&cli2addr, sizeof(cli2addr));
}
