#include <stdio.h>
#include <stdlib.h>

#include "net.h"

int checkParameter(char const *param);

int main(int argc, char const *argv[])
{
    /**
     * runMode = 1 if param is IP
     * runMode = 0 if param is domain
     */
    int runMode;

    if(argc < 2) {
        printf("Too few argument\n");
        exit(1);
    }

    if(argc > 2) {
        printf("Too many argument\n");
        exit(1);
    }

    runMode = checkParameter(argv[1]);
    if(runMode == 1) {
        resolveIpAddress(argv[1]);
    } else if(runMode == 0) {
        resolveDomain(argv[1]);
    }
}

/**
 * @param: a constance string
 * 
 * @return: 1 if is IP, other is 0
 * 
 * check if a string is IP or not 
 */
int checkParameter(char const *param) {
    if(isValidIpAddress(param))
        return 1;
    else return 0;
}
