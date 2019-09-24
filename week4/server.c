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
    @param argv[1]

    @return -1 if invalid port else port number
*/
int getServerPort(const char *port_argument);

/*
    @param message
*/
void die(char *msg);

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

    struct sockaddr_in server_addr, client1_addr, client2_addr;
    char buffer[BUFFER_MAX_LENGTH];

    if(argc < 2) {
        die("Must provide a port number.\nUsage: ./server <PORT Number>\n");
    }

    port = getServerPort(argv[1]);

    if(port == -1) {
        die("Invalid port number.\n");
    }

    return 0;
}

int getServerPort(const char *port_argument) {
    int port_num = atoi(port_argument);

    if(port_num > 0 && port_num < 65535)
        return port_num;
    else
        return -1;    
}

void die(char *msg) {
    perror(msg);
    exit(1);
}