#ifndef DLINKED_H
#define DLINKED_H

#include <stdio.h>
#include <stdlib.h>

typedef struct DLinked
{
    void *value;
    struct DLinked *next;
    struct DLinked *prev;
} DLinked;

DLinked *addD(DLinked *head, void *value);

#endif /* DLINKED_H */
