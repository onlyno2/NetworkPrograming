#include "server.h"

#define MAXLINE 1024 

// Driver code 
int main(int argc, char const *argv[])
{ 
	int sockfd; 
	char buffer[MAXLINE]; 
    int port;
	struct sockaddr_in servaddr, cliaddr; 
	
    if(argc < 2) {
        printf("Must provide a port number.\nUsage: ./server <PORT Number>\n");
        exit(1);
    }

    port = getServerPort(argv[1]);

    if(port == -1) {
        printf("Invalid port number.\n");
        exit(1);
    } 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(port); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

	char buffer_out_char[MAXLINE], buffer_out_digit[MAXLINE];
	int n;
    socklen_t len; 
    while(1) {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
        buffer[n] = '\0'; 
        printf("Client : %s\n", buffer); 
        if(handleRequestBuffer(buffer, buffer_out_char, buffer_out_digit) == -1) {
            printf("Invalid charracter.\n");
            sendto(sockfd, "Error", strlen("Error"), 
                MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
            continue;
        }


        sendto(sockfd, buffer_out_char, strlen(buffer_out_char), 
                MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 

        sendto(sockfd, buffer_out_digit, strlen(buffer_out_digit), 
                MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 

        printf("%s - %s\n", buffer_out_char, buffer_out_digit);

        // reset buffer
        memset(buffer_out_char, 0, strlen(buffer_out_char) + 1);
        memset(buffer_out_digit, 0, strlen(buffer_out_digit) + 1);
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

int handleRequestBuffer(char * buffer_in, char * buffer_out_char, 
        char * buffer_out_digit) 
{
    int char_index = 0;
    int digit_index = 0;

    for(int i = 0; i < strlen(buffer_in); i++) {
        if(isdigit(buffer_in[i]))
            buffer_out_digit[digit_index++] = buffer_in[i];
        else if(isalpha(buffer_in[i]))
            buffer_out_char[char_index++] = buffer_in[i];
        else
            return -1;
    }

    buffer_out_digit[digit_index] = '\0';
    buffer_out_char[char_index] = '\0';

    return 1;
}

