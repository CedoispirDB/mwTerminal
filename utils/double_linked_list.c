#include "double_linked_list.h"

DLinked *addD(DLinked *head, void *value)
{
    DLinked *temp = malloc(sizeof(DLinked));

    temp->value = value;
    temp->next = NULL;
    temp->prev = NULL;

    if (head == NULL)
    {
        return temp;
    }

    DLinked *current = head;
    DLinked *last;

    while (current != NULL)
    {
        last = current;
        current = current->next;
    }
    last->next = temp;
    temp->prev = last;

    return temp;
}