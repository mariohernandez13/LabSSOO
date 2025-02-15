#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    printf("Id del proceso: %ld\n", (long) getpid());
    printf("Id del proceso padre: %ld\n", (long) getppid());
    printf("Id del usuario propietario: %ld\n", (long) getuid());
    return (0);
}