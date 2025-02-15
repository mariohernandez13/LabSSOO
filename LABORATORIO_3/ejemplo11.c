#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;

    pid = fork();
    
    if(pid == 0)
    {
        //Este bloque es ejecutado por el proceso hijo
        printf("Soy el proceso hijo\n");
        printf("Mi pid es: %d\n", getpid());
        printf("El pid de mi proceso padre es: %d\n", getppid());
    }
    else if(pid > 0)
    {
        //Este bloque es ejecutado por el proceso padre
        printf("Soy el proceso padre\n");
        printf("Mi pid es: %d\n", getpid());
        printf("El pid de mi proceso padre es: %d\n", getppid());
    }
    else
    {
        //Este bloque de código es ejecutado cuando el fork() da un error al no poder crear otro proceso 
        perror("Error al crear el proceso");
    }
    
    return (0);
}