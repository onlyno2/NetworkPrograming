// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_MAX_LENGTH 1024
struct sockaddr_in servaddr;

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

    sender function
*/
void sender(int sockfd)
{
    char buffer[BUFFER_MAX_LENGTH];

    printf("%d\n", servaddr.sin_port);
    /* send confirm message */
    if (sendto(sockfd, "send", strlen("send"),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr)) == -1)
    {
        die("send error", 1);
    }
    while (1)
    {
        /* input message */
        printf("Message to sent: ");
        gets(buffer);
        /* check null buffer */
        if (buffer[0] == '\n' || buffer[0] == '\0')
        {
            if (sendto(sockfd, "exit", strlen("exit"),
                       MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                       sizeof(servaddr)) == -1)
            {
                die("send error", 1);
            }
            die("Exit", 0);
        }

        /* send message */
        if (sendto(sockfd, buffer, strlen(buffer),
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr)) == -1)
        {
            die("send error", 1);
        }
    }
}

/*
    @param socket file descriptor

    receiver function
*/
void receiver(int sockfd)
{
    int n;
    socklen_t len;
    char buffer[BUFFER_MAX_LENGTH];

    /* send confirm message */
    if (sendto(sockfd, "recv", strlen("recv"),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr)) == -1)
    {
        die("send error", 1);
    }

    len = sizeof(servaddr);
    while (1)
    {
        if ((n = recvfrom(sockfd, (char *)buffer, BUFFER_MAX_LENGTH,
                          MSG_WAITALL, (struct sockaddr *)&servaddr, &len)) == -1)
        {
            die("error", 1);
        }
        buffer[n] = '\0';
        /* check exit message */
        if (strcmp(buffer, "exit") == 0)
        {
            die("Exit\n", 0);
        }
        if(n != 0)
            printf("%s\n", buffer);
    }
}

// Driver code
int main(int argc, char const *argv[])
{
    int sockfd, mode, port;

    if (argc < 3)
    {
        die("Must provide a port number.\nUsage: ./client <Server Address> <PORT Number>\n", 1);
    }

    if (argc < 2)
    {
        die("Must provide a server address.\nUsage: ./client <Server Address> <PORT Number>\n", 1);
    }

    port = getServerPort(argv[2]);

    if (port == -1)
    {
        die("Invalid port number.\n", 1);
    }

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        die("socket creation failed", 1);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = port;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    printf("1. Sender\n2.Receiver\n");
    printf("Select mode: ");
    scanf("%d%*c", &mode);

    switch (mode)
    {
    case 1:
        printf("sender\n");
        sender(sockfd);
        break;
    case 2:
        printf("revceiver\n");
        receiver(sockfd);
        break;
    default:
        break;
    }

    close(sockfd);
    return 0;
}
