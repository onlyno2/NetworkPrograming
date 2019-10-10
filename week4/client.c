// udp client driver program 
#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include<netinet/in.h> 
#include<unistd.h> 
#include<stdlib.h> 

#define PORT 5000 
#define MAXLINE 1000 


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

// Driver code 
int main() 
{ 
	char buffer[100]; 
	char *message = "Hello Server"; 
	int sockfd; 
	struct sockaddr_in servaddr; 
	
	// clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 
	
	// create datagram socket 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	
	// connect to server 
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{ 
		printf("\n Error : Connect Failed \n"); 
		exit(0); 
	} 

	socklen_t len = sizeof(servaddr);
	/* Send connect infomation */
	sendto(sockfd, message, MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	/* Get connect verify */
	int n;
	if((n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, &len)) == -1) {
		err_message("Recv Error", 1);
	}
	puts(buffer); 

	// close the descriptor 
	close(sockfd); 
} 
