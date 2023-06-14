#include <stdio.h>
#include <stdlib.h>

typedef struct Dog
{
    char *name;
} Dog;

typedef struct Person
{
    char *name;
    int age;
    Dog dog;
} Person;

Dog create_dog(char *name)
{
    Dog p;
    p.name = name;
    return p;
}

Person create_person(char *name, int age, Dog dog)
{
    Person p;
    p.name = name;
    p.age = age;
    p.dog = dog;

    return p;
}

// void change_person_name(Person p, char *new_name)
// {
//     p.name = new_name;
// }

// void change_person_dog(Person p, char *new_name)
// {
//     p.dog.name = new_name;
// }
void print_dog(Dog d)
{
    printf("Dog Name: %s\n", d.name);
}

void print_person(Person p)
{
    printf("Name: %s\n", p.name);
    printf("Age: %d\n", p.age);
    print_dog(p.dog);
}

typedef struct W
{
    int *arr;
} W;

void change_num(W w)
{
    w.arr[1] = 10;
}

int main(void)
{
    W w;
    size_t len = 3;
    w.arr = malloc(len * sizeof(int));

    for (size_t i = 0; i < len; ++i)
    {
        w.arr[i] = i + 1;
    }

    for (size_t i = 0; i < len; i++)
    {
        printf("%d ", w.arr[i]);
    }
    printf("\n");

    change_num(w);

    for (size_t i = 0; i < len; i++)
    {
        printf("%d ", w.arr[i]);
    }
    printf("\n");

    free(w.arr);

    return 0;
}