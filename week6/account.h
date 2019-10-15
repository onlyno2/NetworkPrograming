#include <stdlib.h>
#include <string.h>

/**
 * Maximum number of characters in Accountname
 */
#define MAX_ACCOUNT_LEN 20
/**
 * Maximum number of characters in password
 */
#define MAX_PASSWORD_LEN 20

struct AccountNode_ {
    char* username;
    char* password;
    int status;
    struct AccountNode_ *next;
};

typedef struct AccountNode_ AccountNode;

AccountNode* createAccountNode(char* Accountname, char* password, int status);
void freeAccountNode(AccountNode *node);
void freeAccountsNode(AccountNode *head);
void printAccountNode(AccountNode *head);
void insertAccountNode(AccountNode **head, AccountNode *node);
AccountNode* searchAccountNode(AccountNode *head, char *Accountname);
int isNULL(AccountNode *head);