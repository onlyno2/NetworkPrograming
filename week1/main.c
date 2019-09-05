#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"

/**
 * Store Users data
 */
UserNode *userList = NULL;
/**
 * Current logged in user
 */
UserNode *currentUser;

void readDataStream(char *fileName, UserNode **head);
UserNode *formatLine(char *line);
void menuRegister(void);
void menuSignIn(void);
void menuSearch(void);
void menuSignOut(void);
void updateBlockStatusToFile(char* filename, UserNode *head);

int main(int argc, char const *argv[])
{
    char choice;
    /*
        Read data from file
    */
    readDataStream("account.txt", &userList);
    do
    {
        printf("\nUSER MANAGEMENT PROGRAM\n");
        printf("-------------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign Out\n");
        printf("Your choice (1-4, other to quit): ");
        scanf("%c%*c", &choice);

        switch (choice)
        {
        case '1':
            menuRegister();
            break;
        case '2':
            menuSignIn();
            break;
        case '3': 
            menuSearch();
            break;
        case '4':
            menuSignOut();
            break;        
        default:
            exit(0);
            break;
        }
    } while (1);
    freeUsersNode(userList);
    return 0;
}

/**
 * @param file name, list head
 * 
 * Read data from a file to UserNode* list
 */
void readDataStream(char *fileName, UserNode **head) {
    char username[MAX_USERNAME_LEN], password[MAX_PASSWORD_LEN], status[2];
    FILE *f = fopen(fileName, "rt");
    UserNode* node;
    if(f == NULL) {
        printf("No file data file included. Exit\n");
        exit(1);
    }
        
    while(!feof(f)) {
        fscanf(f, "%s %s %s\n", username, password, status);
        node = createUserNode(username, password, atoi(status));
        insertUserNode(head, node);
    }
    fclose(f);
}

void menuRegister(void) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    printf("\nREGISTER\n");
    /*
        input new data
    */
    printf("Input new username: ");
    scanf("%s%*c", username);
    // if username existed
    if(searchUserNode(userList, username)) {
        printf("User already exist!!\n");
        return;
    }
    printf("Input new password: ");
    scanf("%s%*c", password);

    /*
        insert data to list
    */
    UserNode* newUser = createUserNode(username, password, 1);
    if(newUser)
        insertUserNode(&userList, newUser);
    else
        printf("Register failed");

    /*
        insert data to file
    */
    FILE *f = fopen("account.txt", "a");
    if(f == NULL)
        printf("Cannot open file!!");
    fprintf(f, "%s %s %d\n", username, password, 1);
    fclose(f);
}

void menuSignIn(void) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    printf("\nSIGN IN\n");
    /*
        input data
    */
    printf("Input username: ");
    scanf("%s%*c", username);
    // set session if right user
    currentUser = searchUserNode(userList, username);

    // if username existed
    if(currentUser == NULL) {
        printf("User not found!!\n");
        return;
    }
    
    // check password
    int counter = 0;
    do {
        printf("Input password: ");
        scanf("%s%*c", password);
        if(strcmp(currentUser->password, password) != 0) {
            printf("Password not match!!\n");
            counter++;
        } else
            break;
        if(counter == 3) {
            printf("Log in failed. Your account has been blocked\n\n");
            // update in list
            currentUser->status = 0;
            // update in file
            FILE *f = fopen("account.txt", "w");
            UserNode *curr = userList;
            while(curr) {
                fprintf(f, "%s %s %d\n", curr->username, curr->password, curr->status);
                curr = curr->next;
            }
            curr = NULL;
            fclose(f);
        }
    } while(counter < 3);
    if(counter == 3) {
        currentUser = NULL;
        return;
    } else {
        printf("Log in successed.\n");
    }
}

void menuSearch(void) {
    printf("\nSEARCH\n");
    char username[MAX_USERNAME_LEN];

    printf("Input username to search: ");
    scanf("%s%*c", username);
    UserNode* user = searchUserNode(userList, username);
    if(user == NULL) {
        printf("This user does not exist.\n");
        return;
    } else {
        printf("Username: %s - status: ", user->username);
        if(user->status == 1)
            printf("active.\n");
        else
            printf("blocked.\n");
    }
}

void menuSignOut(void) {
    printf("\nLOG OUT\n");
    char username[MAX_USERNAME_LEN];

    // check if not logged in
    if(currentUser == NULL) {
        printf("Not logged in\n");
        return;
    }

    // if logged in
    printf("Input current username to log out: ");
    scanf("%s%*c", username);
    // check input user with current user
    if(strcmp(currentUser->username, username) == 0) {
        currentUser = NULL;
        printf("Log out successed.\n");
        return;
    } else {
        printf("Wrong user. Please try again later.\n");
        return;
    }
}