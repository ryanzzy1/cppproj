#include<stdio.h>

int Callback_1() // Callback Function 1
{
    printf("Hello, this is Callback_1\n ");
    return 0;
}

int Callback_2() // Callback Function 2
{
    printf("Hello, this is Callback_2\n ");
    return 0;
}

int Callback_3() // Callback Function 3
{
    printf("Hello, this is Callback_3\n ");
    return 0;
}

int Handle(int (*Callback)())
{
    printf("Entering Handle Function.\n ");
    Callback();
    printf("Leaving Handle Function.\n");
}

int main()
{
    printf("Entering Main Function.\n ");
    Handle(Callback_1);
    Handle(Callback_2);
    Handle(Callback_3);
    printf("Leaving Main Function.\n ");
    return 0;
}