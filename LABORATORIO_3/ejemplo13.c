#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main (void)
{
    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == 0)
    {
        // Este es el bloque de código del proceso hijo
        printf("Soy el proceso hijo y mi PID es: %d\n", getpid());
        printf("El PID de mi padre es: %d\n", getppid());

        pid2 = fork();

        if (pid2 == 0)
        {
            // Este es el bloque de código correspondiente al hijo de pid1 (proceso nieto)
            printf("Soy el proceso nieto y mi PID es: %d\n", getpid());
            printf("El PID de mi padre es: %d\n", getppid());
        }
        else if (pid2 > 0)
        {
            // El hijo espera al nieto
            wait(NULL);
            printf("El nieto ha terminado, el proceso hijo continúa\n");
        }
    }
    else if (pid1 > 0)
    {
        // Código del proceso padre
        printf("Soy el proceso padre y mi PID es: %d\n", getpid());
        // El padre espera al hijo
        wait(NULL);
        printf("El hijo ha terminado, el proceso padre continúa.\n");
    }
    else
    {
        // Imprimimos error de creación de proceso 
        perror("Error al crear proceso\n");
    }

    return(0);
}