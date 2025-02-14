#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
printf("ID de proceso: %ld\n", (long) getpid());
printf("ID de proceso padre: %ld\n", (long) getppid());
printf("ID de usuario propietario: %ld\n", (long) getuid());
return 0;
}
