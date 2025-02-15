#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("Soy example_01.3.c, mi PID %d\n", getpid());
    char *args[] = {"example_01.4", "c", "programming", NULL};
    execv("./example_01.4", args);

    printf("Volviendo a example_01.3.c\n");

    return(0);
}