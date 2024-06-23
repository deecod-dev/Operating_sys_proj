#include <stdio.h>
#include <stdlib.h>

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    if (n < 0)
    {
        printf("negative integer not allowed.\n");
        return 1;
    }
    printf("Fibonacci(%d) = %d\n", n, fibonacci(n));
    return 0;
}