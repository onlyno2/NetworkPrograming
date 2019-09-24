#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

/*
    @param source message
    @param error message check

    @return 0 if is error

    Check server message is error or not
*/
int checkErrorMessage(char *source, char *err_message);


/*
    @param string source

    @return 1 if a valid address, 0 if invalid

    Determine if a source is valid ip address
*/
int isValidIpAddress(const char *source);

/* 
    @param port number from command argument

    @return port number
*/
int getServerPort(const char *port_argument);
