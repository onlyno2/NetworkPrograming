#include "client.h"

#define DEFAULT_PORT 8000
#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define MAXLINE 1024

struct sockaddr_in servaddr;
// Driver code
int main(int argc, char const *argv[])
{
    int port, sockfd;
    char buffer[MAXLINE];

    if(argc < 3) {
        printf("Too few argument.\nUsage: ./client <IP Address> <PORT>\n");
        exit(1);
    }

    
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if(!isValidIpAddress(argv[1])) {
        printf("Invalid IP address. Using default IP.\n");
        inet_aton(DEFAULT_IP_ADDRESS, &servaddr.sin_addr);
    } else {
        inet_aton(argv[1], &servaddr.sin_addr);
    }

    port = getServerPort(argv[2]);
    printf("Connected to server: %s:%d\n", argv[1], port);
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int n;
    socklen_t len;
    while (1)
    {
        getchar();
        printf("Message to sent: ");
        scanf("%s%*c", buffer);
        if(sendto(sockfd, (const char *)buffer, strlen(buffer),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
                   perror("Message sending error: ");
                   continue;
               }
        printf("Message sent.\n");
        memset(buffer, 0, MAXLINE);
        
        if((n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
            0, (struct sockaddr *)&servaddr, &len)) == -1) {
                perror("Message revc error: ");
                continue;
            }
        buffer[len] = '\0';
        printf("%s\n", buffer);

        /* Check if not error message returned */
        if(checkErrorMessage(buffer, "Error") == 0) {
           continue;
        }

        memset(buffer, 0, MAXLINE);

        if((n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
            0, (struct sockaddr *)&servaddr, &len)) == -1) {
                perror("Message revc error: ");
                continue;
            }
        buffer[len] = '\0';
        printf("%s\n", buffer);
        memset(buffer, 0, MAXLINE);
    }
    

    close(sockfd);
    return 0;
}


int checkErrorMessage(char *source, char *err_message) {
    return strncmp(source, err_message, strlen(err_message) - 2);
}

int isValidIpAddress(const char *source) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, source, &(sa.sin_addr));
    return result != 0;
}

int getServerPort(const char * port_argument) {
    int port = atoi(port_argument);
    if(port > 0 && port < 65535) 
        return port;
    else
        return -1;
}
