#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* 
    @param port number from command argument
    @param port

    @return port number
*/
int getServerPort(const char *port_argument);

/*
    @param buffer_in
    @param buffer_out_char = 0 set
    @param buffer_out_digit = 0 set

    Handle buffer getted from client
*/
int handleRequestBuffer(char * buffer_in, char * buffer_out_char, 
        char * buffer_out_digit);




