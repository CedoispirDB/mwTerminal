#include "linkedList.h"

char *DATA_TYPE;

Node *createNode()
{
    return (Node *)malloc(sizeof(Node));
}

Node *add(Node *head, void *value)
{
    Node *temp = createNode();
    temp->value = value;
    temp->next = NULL;

    if (head == NULL)
    {
        return temp;
    }

    Node *current = head;
    Node *last;

    while (current != NULL)
    {
        last = current;
        current = current->next;
    }
    last->next = temp;

    return temp;
}

int len(Node *head)
{

    if(head == NULL) 
    {
        return -1;
    }

    int size = 0;

    Node *current = head;

    while (current != NULL)
    {
        size += 1;
        current = current->next;
    }

    return size;
}

void exitIfOutOfBounds(Node *head, int index, char *loc)
{
    if (index < 0 || index >= len(head))
    {
        printf("Index %i out of bounds in %s\n", index, loc);
        exit(1);
    }
}

void *get(Node *head, int index)
{
    exitIfOutOfBounds(head, index, "get");

    Node *current = head;
    int count = 0;

    while (current != NULL)
    {
        if (count == index)
            return current->value;

        current = current->next;
        count++;
    }

    return NULL;
}

Node *getNode(Node *head, int index)
{
    exitIfOutOfBounds(head, index, "getNode");

    Node *current = head;
    int count = 0;

    while (current != NULL)
    {
        if (count == index)
            return current;

        current = current->next;
        count++;
    }

    return NULL;
}

Node *insert(Node *head, void *value, int index)
{
    exitIfOutOfBounds(head, index, "insert");

    Node *newNode = createNode();

    if (index == 0)
    {
        newNode->value = head->value;
        newNode->next = head->next;
        head->value = value;
        head->next = newNode;
        return newNode;
    }
    else if (index == len(head) - 1)
    {
        add(head, value);
        return newNode;
    }

    newNode->value = value;

    Node *prevNode = getNode(head, index - 1);
    Node *nextNode = getNode(head, index);

    // printf("prevNode - ptr: %p value: %i, next: %p", (void *) prevNode, *(int*)(prevNode->value), (void *)(prevNode->next));
    // printf("nextNode - ptr: %p value: %i, next: %p", (void *) nextNode, *(int*)(nextNode->value), (void *)(nextNode->next));

    prevNode->next = newNode;
    newNode->next = nextNode;

    return newNode;
}

void delete(Node *head, int index)
{
    exitIfOutOfBounds(head, index, "delete");

    Node *remove = getNode(head, index);

    if (index == 0)
    {
        Node *nextNode = head->next;
        head->value = nextNode->value;
        head->next = nextNode->next;
        free(nextNode);
        return;
    }

    Node *prevNode = getNode(head, index - 1);

    prevNode->next = remove->next;

    free(remove);
}

void freeList(Node *head)
{
    Node *current = head;
    Node *temp;
    
    while (current != NULL)
    {
        temp = current;
        current = current->next;

        free(temp);
    }
}

Node *convertArray(void *array, size_t len)
{
    Node *head = NULL;

    if (strcmp(DATA_TYPE, "int") == 0)
    {
        int *intArray = (int *)array;

        for (size_t i = 0; i < len; ++i)
        {
            if (i == 0)
            {
                head = add(NULL, (void *)&intArray[i]);
                continue;
            }
            add(head, (void *)&intArray[i]);
        }
    }
    else if (strcmp(DATA_TYPE, "str") == 0)
    {
        char **strArray = (char **)array;

        for (size_t i = 0; i < len; ++i)
        {
            if (i == 0)
            {
                head = add(NULL, (void *)strArray[i]);
                continue;
            }
            add(head, (void *)strArray[i]);
        }
    }
    else if (strcmp(DATA_TYPE, "char") == 0)
    {
        char *charArray = (char *)array;

        for (size_t i = 0; i < len; ++i)
        {
            if (i == 0)
            {
                head = add(NULL, &charArray[i]);
                continue;
            }
            add(head, &charArray[i]);
        }
    }
    else
    {
        printf("Data type \"%s\" not implemented\n", DATA_TYPE);
    }

    return head;
}

void printValues(Node *head)
{
    printf("Size: %i\n", len(head));
    for (int i = 0; i < len(head); ++i)
    {
        Node *current = getNode(head, i);

        if (strcmp(DATA_TYPE, "int") == 0)
        {
            printf("Value at index %i: %i (%s)\n", i, *(int *)current->value, DATA_TYPE);
        }
        else if (strcmp(DATA_TYPE, "str") == 0)
        {
            printf("Value at index %i: %s (%s)\n", i, (char *)current->value, DATA_TYPE);
        }
        else if (strcmp(DATA_TYPE, "char") == 0)
        {
            printf("Value at index %i: %c (%s)\n", i, *(char *)current->value, DATA_TYPE);
        }
        else
        {
            printf("Data type \"%s\" not implemented\n", DATA_TYPE);
        }
    }
}

char *createStr(char *src)
{
    size_t len = strlen(src) + 1;
    char *des = malloc(len * sizeof(char));
    if (des != NULL)
    {
        memcpy(des, src, len);
    }

    return des;
}

void ShowBasicCreation()
{
    Node *head = add(NULL, &(int){1});
    add(head, &(int){2});
    add(head, &(int){3});
    add(head, &(int){4});
    add(head, &(int){5});

    printf("Values added to list:\n");
    printValues(head);
    printf("\n");
    
    int index = 1;
    printf("Value inserted at index %i:\n", index);
    insert(head, &(int){10}, index);
    printValues(head);
    printf("\n");

    int rmIndex = 3;
    printf("Value deleted at index %i:\n", rmIndex);
    delete (head, rmIndex);
    printValues(head);

    freeList(head);
}

void ShowConvertFromArray(void)
{
    // Create integer array
    int integers[] = {50, 40, 30, 20, 10, 9, 8, 7, 6};
    size_t integerLen = sizeof(integers) / sizeof(integers[0]);

    // Create string array
    char **strings;
    size_t stringLen = 5;
    strings = (char **)malloc(stringLen * sizeof(char *));
    // Allocate memory for each string dynamically
    strings[0] = createStr("Marco");
    strings[1] = createStr("Bia");
    strings[2] = createStr("Bruno");
    strings[3] = createStr("Augie");
    strings[4] = createStr("Bianca");

    // Create char array
    char charArray[6] = {'H', 'e', 'l', 'l', 'o', '!'};
    size_t charLen = sizeof(charArray) / sizeof(charArray[0]);

    Node *integerHead = convertArray(integers, integerLen);
    Node *stringHead = convertArray(strings, stringLen);
    Node *charHead = convertArray(charArray, charLen);

    printf("Converting from int array:\n");
    printValues(integerHead);

    printf("\nConverting from string array:\n");
    printValues(stringHead);

    printf("\nConverting from char array:\n");
    printValues(charHead);

    // Free allocated memory
    freeList(integerHead);
    freeList(stringHead);
    freeList(charHead);
}