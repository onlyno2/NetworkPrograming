#include <unistd.h>
#include <errno.h>
#include <netdb.h> //
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int isValidIpAddress(char const *ipAddress);
void resolveIpAddress(char const *ipAddress);
void resolveDomain(char const *domain);

int checkHostentResult(struct hostent *host);