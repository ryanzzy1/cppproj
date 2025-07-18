// #include<iostream>
#include <stdio.h>

int test(int count)
{
    int i = 0;
    int n = (count + 7) / 8;

    switch (count % 8)
    {
    case 0: do { i++;
    case 7:      i++;
    case 6:      i++;
    case 5:      i++;
    case 4:      i++;
    case 3:      i++;
    case 2:      i++;
    case 1:      i++;
            }while( --n > 0);
        /* code */   
    }
    return i; 
}


int main()
{
    printf("%d\n", test(20));

    return 0;
}