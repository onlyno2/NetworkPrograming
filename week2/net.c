#include "net.h"

/**
 * @param a string
 * 
 * check if given string is a Ip Address or not 
 */
int isValidIpAddress(char const *str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str, &(sa.sin_addr)) != 0;
}

/**
 * @param a hostent return by get
 * 
 * check if a host resolved or not
 */
int checkHostentResult(struct hostent *host) {
    if(host)
        return 1;
    return 0;
}

/**
 * @param string of an ipAddress
 * 
 * resolve a given ipAddess
 */
void resolveIpAddress(char const *ipAddress) {
    /**
     * contain an Ipv4addr
     */
    struct in_addr ipV4Address; 
    /**
     * contain result
     */
    struct hostent *host_entry;

    // retrive checkHost value
    inet_pton(AF_INET, ipAddress, &ipV4Address);
    host_entry = gethostbyaddr(&ipV4Address, sizeof(ipV4Address), AF_INET);
    if(!checkHostentResult(host_entry)) {
        printf("Not found information\n");
        exit(1);
    }
    printf("Official Name: %s\n", host_entry->h_name);
    
    // print aliases
    if (host_entry->h_aliases) {
        printf("Alias name:\n");
        for(int i = 0; host_entry->h_aliases[i] != NULL; i++) {
            printf("%s\n", host_entry->h_aliases[i]);
        }
    }
}   

/**
 * @param string of a domain
 * 
 * resolve a given domain 
 */
void resolveDomain(char const *domain) {
    /**
     * contain result
     */
    struct hostent *host_entry;

    host_entry = gethostbyname(domain);
    if(!checkHostentResult(host_entry)) {
        printf("Not found information\n");
        exit(1);
    }
    
    /**
     * contain returned address list 
     */
    struct in_addr **addr_list = (struct in_addr **)host_entry->h_addr_list;
    printf("Official IP: %s\n", inet_ntoa(*addr_list[0]));
    if(addr_list[1]) {
        printf("Alias IP:\n");
        for(int i = 1; addr_list[i] != NULL; i++) {
            printf("%s\n", inet_ntoa(*addr_list[i]));
        }
    }
}