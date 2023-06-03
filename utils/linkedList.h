#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define EXIT_IF_OUT_OF_BOUNDS(head, index, loc)                       \
    do                                                                \
    {                                                                 \
        if ((index) < 0 || (index) >= len(head))                      \
        {                                                             \
            printf("Index %i out of bounds on %s function in line %d\n", (index), __func__, __LINE__); \
            exit(-1);                                                 \
        }                                                             \
    } while (0)

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
void delete(Node **head, int index);
void freeList(Node *head);
void printValues(Node *head);
void ShowBasicCreation();
void ShowConvertFromArray();
char *createStr(char *src);
int len(Node *head);

#endif // CUSTOM_LIST_H
