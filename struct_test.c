#include <stdio.h>

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

int main(void)
{
    Dog dog = create_dog("Leo");
    Person p = create_person("Marco", 19, dog);

    print_person(p);

    return 0;
}