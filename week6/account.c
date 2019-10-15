#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "account.h"

/**
 * @param head of a AccountNode* list
 * 
 * @return 1 if list is NULL or 0 if list is not NULL
 * 
 * Check a list is NULL or not
 */
int isNULL(AccountNode *head)
{
    if (head->username == NULL)
        return 1;
    return 0;
}

/**
 * @params Accountname, password and status for a Account
 * 
 * @return created AccountNode*
 * 
 * Create a AccountNode
 */
AccountNode *createAccountNode(char *Accountname, char *password, int status)
{
    if (strlen(Accountname) > MAX_ACCOUNT_LEN || strlen(password) > MAX_PASSWORD_LEN)
        return NULL;

    AccountNode *newAccountNode = (AccountNode *)malloc(sizeof(AccountNode));
    newAccountNode->username = (char *)malloc(sizeof(char) * (strlen(Accountname) + 1));
    strcpy(newAccountNode->username, Accountname);
    newAccountNode->password = (char *)malloc(sizeof(char) * (strlen(password) + 1));
    strcpy(newAccountNode->password, password);
    newAccountNode->status = status;
    return newAccountNode;
}

/**
 * @param head of a AccountNode* list
 * 
 * Print AccountNode list passed in
 */
void printAccountNode(AccountNode *head)
{
    if (isNULL(head))
    {
        return;
    }
    while (head != NULL)
    {
        printf("%s - %s - %d\n", head->username, head->password, head->status);
        head = head->next;
    }
    printf("\n");
}

/**
 * @param a AccountNode* node
 * 
 * Free a AccountNode* node
 */
void freeAccountNode(AccountNode *node)
{
    if (node)
    {
        free(node->username);
        free(node->password);
        free(node);
        node = NULL;
    }
}

/**
 * @param head of a AccountNode* list
 * 
 * Free a AccountNode* list
 */
void freeAccountsNode(AccountNode *head)
{
    AccountNode *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        freeAccountNode(tmp);
    }
}

/**
 * @params head of a AccountNode** list, a AccountNode* node to insert
 * 
 * Insert a AccountNode* into head of a AccountNode* list
 */
void insertAccountNode(AccountNode **head, AccountNode *node)
{
    if (*head == NULL)
    {
        *head = node;
        return;
    }
    AccountNode *tmp = *head;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = node;
}

/** 
 * @params head of a AccountNode* list, Accountname to find
 * 
 * @return a AccountNode* if found, NULL if not found
 * 
 * Find a AccountNode* in a AccountNode* list
 */
AccountNode *searchAccountNode(AccountNode *head, char *Accountname)
{
    if (isNULL(head))
        return NULL;

    while (head != NULL)
    {
        if (strcmp(head->username, Accountname) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}