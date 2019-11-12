#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_MAX_LENGTH 1024

/* 
    cli1: sender
    cli2: receiver

    clitmp: get request from clint and define sender and receiver
    servadd: server address
*/
struct sockaddr_in cli1, cli2, clitmp, servaddr;

/*
    @param port number in char

    @return -1 if invalid port else port number
*/
int getServerPort(const char *port_argument);

/*
    @param message string

    exit program when exit encontered
*/
void die(char *msg, int type);

/*
    @param client 1
    @param client 2

    @return 1 if equal, 0 if not equal
    
    compare 2 client info
*/
int clientCompare(struct sockaddr_in cli1, struct sockaddr_in cli2)
{
    if ((cli1.sin_addr.s_addr == cli2.sin_addr.s_addr) && (cli1.sin_port == cli2.sin_port))
        return 1;
    return 0;
}

/*
    @param socket descriptor
    
    get client info
*/
void getClientInfo(int sockfd)
{
    int n;
    socklen_t len = sizeof(clitmp);
    char buffer[BUFFER_MAX_LENGTH];
    printf("Getting client info\n");

    /* handle client 1st info */
    if ((n = recvfrom(sockfd, (char *)buffer, BUFFER_MAX_LENGTH,
                      MSG_WAITALL, (struct sockaddr *)&clitmp, &len)) == -1)
    {
        die("error: get client info", 1);
    }
    buffer[n] = '\0';
    if (strcmp(buffer, "send") == 0)
    {
        cli1 = clitmp;
        printf("Getted client 1 info\n");
    }
    if (strcmp(buffer, "recv") == 0)
    {
        cli2 = clitmp;
        printf("Getted client 1 info\n");
    }

    /* handle client 2nd info */
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        if ((n = recvfrom(sockfd, (char *)buffer, BUFFER_MAX_LENGTH,
                          MSG_WAITALL, (struct sockaddr *)&clitmp, &len)) == -1)
        {
            die("error: get client info", 1);
        }
        buffer[n] = '\0';
        if (clientCompare(cli1, clitmp) || clientCompare(cli2, clitmp))
            continue;
        else if (clientCompare(cli1, clitmp) == 0)
        {
            cli2 = clitmp;
            printf("Getted client 2 info\n");
            break;
        }
        else if (clientCompare(cli2, clitmp) == 0)
        {
            cli1 = clitmp;
            printf("Getted client 1 info\n");
            break;
        }
    }
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
    return 1;
}

/*
    @param string contain only character
    @param string contain only digit

    @return string_char + '\n' + string_digit

    format message send to receiver
*/
char *createMessage(const char *string_char, const char *string_digit)
{
    char *returnMessage = malloc(strlen(string_char) + strlen(string_digit) + 2);
    memset(returnMessage, 0, strlen(returnMessage));
    strcpy(returnMessage, string_char);
    strcat(returnMessage, "\n");
    strcat(returnMessage, string_digit);

    return returnMessage;
}

/*
    @param socket file descriptor

    get buffer from sender and send formated message to receiver
*/
void handleRequest(int sockfd)
{
    int n;
    socklen_t len;
    char buffer[BUFFER_MAX_LENGTH], buffer_out_char[BUFFER_MAX_LENGTH], buffer_out_digit[BUFFER_MAX_LENGTH];
    while (1)
    {
        memset(buffer_out_char, 0, BUFFER_MAX_LENGTH);
        memset(buffer_out_digit, 0, BUFFER_MAX_LENGTH);
        printf("Waiting for message.... \n");
        len = sizeof(cli1);
        if ((n = recvfrom(sockfd, (char *)buffer, BUFFER_MAX_LENGTH,
                          MSG_WAITALL, (struct sockaddr *)&cli1, &len)) == -1)
        {
            die("error: ", 1);
        }
        buffer[n] = '\0';
        printf("Buffer: %s\n", buffer);
        /* if got message exit */
        if(strcmp(buffer, "exit") == 0) {
            if (sendto(sockfd, "exit", strlen("exit"),
                       MSG_CONFIRM, (const struct sockaddr *)&cli2,
                       sizeof(cli2)) == -1)
            {
                die("send error", 1);
            }
        }
        
        len = sizeof(cli2);
        /* handle message */
        if ((n = handleBuffer(buffer, buffer_out_char, buffer_out_digit)) == -1)
        {
            if (sendto(sockfd, "Error: Invalid character", strlen("Error: Invalid character"),
                       MSG_CONFIRM, (const struct sockaddr *)&cli2,
                       sizeof(cli2)) == -1)
            {
                die("send error", 1);
            }
        }
        else
        {
            if (sendto(sockfd, createMessage(buffer_out_char, buffer_out_digit),
                       strlen(buffer_out_char) + strlen(buffer_out_digit) + 2,
                       MSG_CONFIRM, (const struct sockaddr *)&cli2,
                       sizeof(cli2)) == -1)
            {
                die("send error", 1);
            }
        }
    }
}

int main(int argc, char const *argv[])
{

    /*
        server ports
    */
    int port;
    /*
        socket file descriptor
    */
    int sockfd;

    if (argc < 2)
    {
        die("Must provide a port number.\nUsage: ./server <PORT Number>\n", 1);
    }

    port = getServerPort(argv[1]);

    if (port == -1)
    {
        die("Invalid port number.\n", 1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        die("socket creation failed", 1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cli1, 0, sizeof(cli1));
    memset(&cli2, 0, sizeof(cli2));
    memset(&clitmp, 0, sizeof(clitmp));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = port;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        die("bind error", 1);
    }

    getClientInfo(sockfd);

    handleRequest(sockfd);

    close(sockfd);
    return 0;
}

int getServerPort(const char *port_argument)
{
    int port_num = atoi(port_argument);

    if (port_num > 0 && port_num < 65535)
        return port_num;
    else
        return -1;
}

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