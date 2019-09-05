#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "user.h"

/**
 * @param head of a UserNode* list
 * 
 * @return 1 if list is NULL or 0 if list is not NULL
 * 
 * Check a list is NULL or not
 */
int isNULL(UserNode* head) {
    if(head->username == NULL)
        return 1;
    return 0;
}


/**
 * @params username, password and status for a User
 * 
 * @return created UserNode*
 * 
 * Create a UserNode
 */
UserNode* createUserNode(char* username, char* password, int status) {
    if(strlen(username) > MAX_USERNAME_LEN || strlen(password) > MAX_PASSWORD_LEN)
        return NULL;

    UserNode* newUserNode = (UserNode*)malloc(sizeof(UserNode));
    newUserNode->username = (char*)malloc(sizeof(char) * (strlen(username) + 1));
    strcpy(newUserNode->username, username);
    newUserNode->password = (char*)malloc(sizeof(char) * (strlen(password) + 1));
    strcpy(newUserNode->password, password);
    newUserNode->status = status;
    return newUserNode;
}

/**
 * @param head of a UserNode* list
 * 
 * Print UserNode list passed in
 */
void printUserNode(UserNode *head) {
    if(isNULL(head)) {
        return;
    }
    while(head != NULL) {
        printf("%s - %s - %d\n", head->username, head->password, head->status);
        head = head->next;
    }
    printf("\n");
}

/**
 * @param a UserNode* node
 * 
 * Free a UserNode* node
 */
void freeUserNode(UserNode *node) {
    if(node) {
        free(node->username);
        free(node->password);
        free(node);
        node = NULL;
    }
}

/**
 * @param head of a UserNode* list
 * 
 * Free a UserNode* list
 */
void freeUsersNode(UserNode *head) {
    UserNode *tmp; 
    while(head != NULL) {
        tmp = head;
        head = head->next;
        freeUserNode(tmp);
    }
}

/**
 * @params head of a UserNode** list, a UserNode* node to insert
 * 
 * Insert a UserNode* into head of a UserNode* list
 */
void insertUserNode(UserNode **head, UserNode *node) {
    if((*head) == NULL)
        *head = node;
    else {
        UserNode *curr = *head;
        while(curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = node;
    }
}

/** 
 * @params head of a UserNode* list, username to find
 * 
 * @return a UserNode* if found, NULL if not found
 * 
 * Find a UserNode* in a UserNode* list
 */
UserNode* searchUserNode(UserNode* head, char* username) {
    if(isNULL(head))
        return NULL;
    
    while(head != NULL) {
        if(strcmp(head->username, username) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}