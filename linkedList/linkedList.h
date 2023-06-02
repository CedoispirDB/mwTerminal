#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

extern char *DATA_TYPE;

typedef struct Node Node;
typedef struct Node LinkedList;

struct Node
{
    void *value;
    Node *next;
};

Node *createNode();
Node *add(Node *head, void *value);
Node *getNode(Node *head, int index);
Node *insert(Node *head, void *value, int index);
Node *convertArray(void *array, size_t len);
void *get(Node *head, int index);
void exitIfOutOfBounds(Node *head, int index, char *loc);
void delete(Node *head, int index);
void freeList(Node *head);
void printValues(Node *head);
void ShowBasicCreation();
void ShowConvertFromArray();
char *createStr(char *src);
int len(Node *head);

#endif  // CUSTOM_LIST_H
