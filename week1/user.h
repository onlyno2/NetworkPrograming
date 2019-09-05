#include <stdlib.h>
#include <string.h>

/**
 * Maximum number of characters in username
 */
#define MAX_USERNAME_LEN 20
/**
 * Maximum number of characters in password
 */
#define MAX_PASSWORD_LEN 20

struct UserNode_ {
    char* username;
    char* password;
    int status;
    struct UserNode_ *next;
};

typedef struct UserNode_ UserNode;

UserNode* createUserNode(char* username, char* password, int status);
void freeUserNode(UserNode *node);
void freeUsersNode(UserNode *head);
void printUserNode(UserNode *head);
void insertUserNode(UserNode **head, UserNode *node);
UserNode* searchUserNode(UserNode *head, char *username);
int isNULL(UserNode *head);

