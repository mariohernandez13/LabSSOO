#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    pid = fork();
    
    if(pid == 0)
    {
        //Este bloque es ejecutado por el proceso hijo
        printf("Soy el hijo. Mi PID es: %d\n", getpid());
        printf("El PID de mi padre es: %d\n", getppid());
        sleep(2);
        printf("El hijo ha terminado\n");
    }
    else if(pid > 0)
    {
        //Este bloque es ejecutado por el proceso padre
        printf("Soy el padre. Mi PID es: %d\n", getpid());
        printf("El PID de mi padre es: %d\n", getppid());
        wait(NULL);
        printf("El proceso hijo ha terminado. Sigo con el padre\n");
    }
    else
    {
        //Este bloque de código es ejecutado cuando el fork() da un error al no poder crear otro proceso 
        perror("Error al crear el proceso");
    }
    
    return (0);
}